#include "sigismund.h"
#include <cstring>
#include <iostream>

static Sigismund app;


//---------------------------------------------------------
//   Sigismund
//---------------------------------------------------------

Sigismund::Sigismund()
{
    ;
}

ErrorCode Sigismund::initialize()
{
    initializeShared();

    /* job handling */
    jobSched = new Scheduler(4);

    return EC_SUCCESS;
}

ErrorCode Sigismund::initialize(int argc, char** argv)
{
    // TODO

    return initializeShared();
}

ErrorCode Sigismund::initializeShared()
{
    /* create indices */
    exactIndex = new Hashtable<const char*, Entry*>(1000, equalStrings);
    editIndex = new BKTree(editDistanceMatching);

    /* create 28 bktrees for hamming distance entries */
    for (unsigned int i = 0; i < MAX_WORD_LENGTH - MIN_WORD_LENGTH + 1; i++) {
        hammingIndex[i] = new BKTree(hammingDistanceMatching);
    }

    /* create hashtables for updating indices */
    editEntries = new Hashtable<const char*, Entry*>(1000, equalStrings);
    hammingEntries = new Hashtable<const char*, Entry*>(1000, equalStrings);

    /* hold queries */
    queries = new Hashtable<QueryID, Query*>(1000, equalIntegers);
    inactiveQueries = new Hashtable<QueryID, QueryID>(1000, equalIntegers);

    /* threading */
    pthread_mutex_init(&results_mtx, NULL);
    sem_init(&results_not_empty, 1, 0);    
    pthread_mutex_init(&queries_mtx, NULL);
    pthread_mutex_init(&exact_mtx, NULL);
    pthread_mutex_init(&edit_mtx, NULL);
    pthread_mutex_init(&entries_mtx, NULL);
    pthread_mutex_init(&hamming_entry_mtx, NULL);
    pthread_mutex_init(&edit_entry_mtx, NULL);

    pthread_mutex_init(&word_counter_mtx, NULL);

    for (unsigned int i = 0; i < MAX_WORD_LENGTH - MIN_WORD_LENGTH + 1; i++) {
        pthread_mutex_init(&hamming_mtx[i], NULL);
    }

    return EC_SUCCESS;
}

ErrorCode Sigismund::destroy()
{
    jobSched->waitForJobsToFinish();

    delete jobSched;

    for (auto& entry : allEntries)
        delete entry;

    for (unsigned int i = 0; i < MAX_WORD_LENGTH - MIN_WORD_LENGTH + 1; i++)
        delete hammingIndex[i];

    delete exactIndex;
    delete editIndex;

    for (auto& x : queries->keys()) {
        delete (*queries->get(x));
    }
    delete queries;
    delete inactiveQueries;

    delete editEntries;
    delete hammingEntries;

    while (results.count() > 0) {
        delete results.first();
        results.remove(static_cast<unsigned int>(0));
    }

    /* threading */
    pthread_mutex_destroy(&results_mtx);
    sem_destroy(&results_not_empty);
    pthread_mutex_destroy(&queries_mtx);
    pthread_mutex_destroy(&exact_mtx);
    pthread_mutex_destroy(&edit_mtx);
    pthread_mutex_destroy(&entries_mtx);
    pthread_mutex_destroy(&hamming_entry_mtx);
    pthread_mutex_destroy(&edit_entry_mtx);

    pthread_mutex_destroy(&word_counter_mtx);

    for (unsigned int i = 0; i < MAX_WORD_LENGTH - MIN_WORD_LENGTH + 1; i++) {
        pthread_mutex_destroy(&hamming_mtx[i]);
    }


    return EC_SUCCESS;
}

//---------------------------------------------------------
//   Sigismund::start
//---------------------------------------------------------

ErrorCode Sigismund::start(Query*& query)
{
    /* remember the Query */
    pthread_mutex_lock(&queries_mtx);
    queries->insert(query->id, query);
    pthread_mutex_unlock(&queries_mtx);

    switch (query->matchType) {
    case MT_EXACT_MATCH:        
        return insertQueryExact(query);
    case MT_HAMMING_DIST:
        return insertQueryHamming(query);
    case MT_EDIT_DIST:
        return insertQueryEdit(query);
    }

    return EC_FAIL;
}

//---------------------------------------------------------
//   addQueryToHashtable
///     Add the given Query to the given Hashtable.
///     If an Entry with the query's word exists then just add
///     the QueryID to that Entry.
///     Otherwise add a new Entry with that QueryID.
//---------------------------------------------------------
ErrorCode Sigismund::addWordToHashtable(const char* word, QueryID id, Hashtable<const char*, Entry*>*& table, Entry*& out_entry)
{
    Entry** entry = table->get(word);
    if (entry) {
        (*entry)->m_appearsIn.append(id);
    } else {
        Entry* newEntry = new Entry(word);
        newEntry->m_appearsIn.append(id);
        table->insert(word, newEntry);
        out_entry = newEntry;

        /* add new entry in entries' list so it can be deleted later */
        pthread_mutex_lock(&entries_mtx);
        allEntries.append(newEntry);
        pthread_mutex_unlock(&entries_mtx);
    }

    return EC_SUCCESS;
}

//---------------------------------------------------------
//   insertQueryExact
///     Insert each word into the Hashtable/index.
//---------------------------------------------------------
ErrorCode Sigismund::insertQueryExact(Query*& query)
{
    char* rest;
    char* token = strtok_r(query->word, " ", &rest);


    while(token != NULL) {
        Entry* newEntry { nullptr };

        pthread_mutex_lock(&exact_mtx);
        ErrorCode error = addWordToHashtable(token, query->id, exactIndex, newEntry);
        pthread_mutex_unlock(&exact_mtx);

        if (error != EC_SUCCESS) {
            return error;
        }

        /* get next word */
        token = strtok_r(NULL, " ", &rest);
    }

    return EC_SUCCESS;
}

//---------------------------------------------------------
//   insertQueryHamming
///     Add a new entry to the BK-tree only if one was created.
///     Otherwise the hashtable will take care of updating the
///     payload.
//---------------------------------------------------------
ErrorCode Sigismund::insertQueryHamming(Query*& query)
{
    char* rest;
    char* token = strtok_r(query->word, " ", &rest);

    while(token != NULL) {
        Entry* newEntry { nullptr };

        pthread_mutex_lock(&hamming_entry_mtx);
        ErrorCode error = addWordToHashtable(token, query->id, hammingEntries, newEntry);
        pthread_mutex_unlock(&hamming_entry_mtx);

        if (error != EC_SUCCESS) {
            return error;
        }

        pthread_mutex_lock(&hamming_mtx[strlen(token) - MIN_WORD_LENGTH]);
        if (newEntry) {
            size_t length = strlen(token);
            hammingIndex[length - MIN_WORD_LENGTH]->insert(newEntry);            
        }
        pthread_mutex_unlock(&hamming_mtx[strlen(token) - MIN_WORD_LENGTH]);

        /* get next word */
        token = strtok_r(NULL, " ", &rest);
    }

    return EC_SUCCESS;
}

//---------------------------------------------------------
//   insertQueryEdit
///     Add a new entry to the BK-tree only if one was created.
///     Otherwise the hashtable will take care of updating the
///     payload.
//---------------------------------------------------------
ErrorCode Sigismund::insertQueryEdit(Query*& query)
{
    char* rest;
    char* token = strtok_r(query->word, " ", &rest);

    while(token != NULL) {
        Entry* newEntry { nullptr };

        pthread_mutex_lock(&edit_entry_mtx);
        ErrorCode error = addWordToHashtable(token, query->id, editEntries, newEntry);
        pthread_mutex_unlock(&edit_entry_mtx);

        if (error != EC_SUCCESS) {
            return error;
        }

        pthread_mutex_lock(&edit_mtx);
        if (newEntry) {            
            editIndex->insert(newEntry);
        }
        pthread_mutex_unlock(&edit_mtx);

        /* get next word */
        token = strtok_r(NULL, " ", &rest);
    }

    return EC_SUCCESS;
}

//---------------------------------------------------------
//   Sigismund::end
//---------------------------------------------------------

ErrorCode Sigismund::end(QueryID queryId)
{
    inactiveQueries->insert(queryId, queryId);
    return EC_SUCCESS;
}

//---------------------------------------------------------
//   Sigismund::match
//---------------------------------------------------------

size_t get_element_from_2d(size_t arrayWidth, int indexA, int indexB)
{
    return indexB * arrayWidth + indexA;
}

ErrorCode Sigismund::match(DocID doc_id, const char* doc_str)
{
    DocResult* doc = new DocResult;
    doc->id = doc_id;

    List<const char*>* words = deduplicate(doc_str);

    auto arrayWidth = (queries->keys().count() + 1);
    auto wordCounterArray = new unsigned int[(queries->keys().count() + 1) * 6]; /* 0-4 bitmap, 5 count */
    memset(wordCounterArray, 0, sizeof(unsigned int) * (queries->keys().count() + 1) * 6); /* initialize to 0 */

    for (auto& x : *words) {
#ifdef DOCUMENT_THREADING
        MatchWordJob(x, wordCounterArray, arrayWidth).execute();
#elif WORD_THREADING
        app.submitJob(new MatchWordJob(x, wordCounterArray, arrayWidth));
#endif
    }

#ifdef WORD_THREADING
    app.jobSched->insertBarrier();
    pthread_barrier_wait(&app.jobSched->m_barrier);
#endif

    for (size_t i = 0; i < queries->keys().count() + 1; i++) {
        if (Query** q = queries->get(i)) {
            if ((*q)->wordsNum == wordCounterArray[get_element_from_2d(arrayWidth, i, 5)]) {
                doc->queries.append(i);
            }
        }
    }

    pthread_mutex_lock(&results_mtx);

    results.append(doc);

    pthread_mutex_unlock(&results_mtx);
    sem_post(&results_not_empty);

    for(auto& x : *words) {
        free((void*)x);
    }
    delete words;
    delete[] wordCounterArray;

    return EC_SUCCESS;
}

//---------------------------------------------------------
//   Sigismund::getResult
//---------------------------------------------------------

ErrorCode Sigismund::getResult(DocResult*& d)
{
    sem_wait(&results_not_empty);
    pthread_mutex_lock(&results_mtx);

    /* get the first result in the list (FIFO) */
    d = results.first();

    /* remove inactive queries from the result */
    for (auto i = d->queries.head()->m_next; i != d->queries.tail(); i = i->m_next) {
        if (inactiveQueries->get(*(i->m_data))) {
            i = i->m_previous;
            d->queries.remove(i->m_next);
        }
    }

    /* remove the result from the list */
    results.remove(static_cast<unsigned int>(0));

    pthread_mutex_unlock(&results_mtx);

    return EC_SUCCESS;
}

//---------------------------------------------------------
//   Jobs
//---------------------------------------------------------

ErrorCode Sigismund::submitJob(Job* job)
{
    jobSched->submitJob(job);
    return EC_SUCCESS;
}

StartJob::StartJob(Query* query) : Job(JOB_TYPE::START_QUERY), m_query(query)
{
    ;
}

StartJob::~StartJob()
{
    // doesn't own the query so doesn't free anything
    ;
}

bool StartJob::execute()
{
    app.start(m_query);
    return true;
}

MatchDocumentJob::MatchDocumentJob(DocID docId, char* docStr) : Job(JOB_TYPE::MATCH)
{
    m_docId = docId;
    m_docStr = docStr;
}

MatchDocumentJob::~MatchDocumentJob()
{
    free(m_docStr);
}

bool MatchDocumentJob::execute()
{
    app.match(m_docId, m_docStr);
    return true;
}

MatchWordJob::MatchWordJob(const char* word, unsigned int*& wordCounterArray, size_t arrayWidth) : Job(JOB_TYPE::MATCH_WORD), wordCounterArray(wordCounterArray)
{
    this->word = word;
    this->arrayWidth = arrayWidth;
}

MatchWordJob::~MatchWordJob()
{
    // doesn't own anything
    ;
}

bool MatchWordJob::execute()
{
    /* Check for Exact Matching */
    if (app.exactIndex->get(word) != nullptr) {
        Entry* e = *app.exactIndex->get(word);

        pthread_mutex_lock(&app.word_counter_mtx);

        for (auto& qid : e->m_appearsIn) {
            wordCounterArray[get_element_from_2d(arrayWidth, qid, 5)] += 1;
        }

        pthread_mutex_unlock(&app.word_counter_mtx);
    }
    /* Check for Edit Distance Matching */
    EntryList* editArray = app.editIndex->multiLookup(word, 1, 3);

    pthread_mutex_lock(&app.word_counter_mtx);

    for (auto& entry : editArray[0]) {
        for (auto& qid : entry->m_appearsIn) {
            if (Query** q = app.queries->get(qid)) {
                int wordPos = *(*q)->wordPos.get(entry->m_word);
                if (wordCounterArray[get_element_from_2d(arrayWidth, qid, wordPos)] == 0) {
                    wordCounterArray[get_element_from_2d(arrayWidth, qid, wordPos)] = 1;
                    wordCounterArray[get_element_from_2d(arrayWidth, qid, 5)] += 1;
                }
            }
        }
    }
    for (auto& entry : editArray[1]) {
        for (auto& qid : entry->m_appearsIn) {
            if (Query** q = app.queries->get(qid)) {
                if ((*q)->matchDist >= 2) {
                    int wordPos = *(*q)->wordPos.get(entry->m_word);
                    if (wordCounterArray[get_element_from_2d(arrayWidth, qid, wordPos)] == 0) {
                        wordCounterArray[get_element_from_2d(arrayWidth, qid, wordPos)] = 1;
                        wordCounterArray[get_element_from_2d(arrayWidth, qid, 5)] += 1;
                    }
                }
            }
        }
    }
    for (auto& entry : editArray[2]) {
        for (auto& qid : entry->m_appearsIn) {
            if (Query** q = app.queries->get(qid)) {
                if ((*q)->matchDist >= 3) {
                    int wordPos = *(*q)->wordPos.get(entry->m_word);
                    if (wordCounterArray[get_element_from_2d(arrayWidth, qid, wordPos)] == 0) {
                        wordCounterArray[get_element_from_2d(arrayWidth, qid, wordPos)] = 1;
                        wordCounterArray[get_element_from_2d(arrayWidth, qid, 5)] += 1;
                    }
                }
            }
        }
    }

    pthread_mutex_unlock(&app.word_counter_mtx);

    delete[] editArray;

    /* Check for Hamming Distance Matching */
    EntryList* hammingArray = app.hammingIndex[strlen(word) - MIN_WORD_LENGTH]->multiLookup(word, 1, 3);

    pthread_mutex_lock(&app.word_counter_mtx);

    for (auto& entry : hammingArray[0]) {
        for (auto& qid : entry->m_appearsIn) {
            if (Query** q = app.queries->get(qid)) {
                int wordPos = *(*q)->wordPos.get(entry->m_word);
                if (wordCounterArray[get_element_from_2d(arrayWidth, qid, wordPos)] == 0) {
                    wordCounterArray[get_element_from_2d(arrayWidth, qid, wordPos)] = 1;
                    wordCounterArray[get_element_from_2d(arrayWidth, qid, 5)] += 1;
                }
            }
        }
    }
    for (auto& entry : hammingArray[1]) {
        for (auto& qid : entry->m_appearsIn) {
            if (Query** q = app.queries->get(qid)) {
                if ((*q)->matchDist >= 2) {
                    int wordPos = *(*q)->wordPos.get(entry->m_word);
                    if (wordCounterArray[get_element_from_2d(arrayWidth, qid, wordPos)] == 0) {
                        wordCounterArray[get_element_from_2d(arrayWidth, qid, wordPos)] = 1;
                        wordCounterArray[get_element_from_2d(arrayWidth, qid, 5)] += 1;
                    }
                }
            }
        }
    }
    for (auto& entry : hammingArray[2]) {
        for (auto& qid : entry->m_appearsIn) {
            if (Query** q = app.queries->get(qid)) {
                if ((*q)->matchDist >= 3) {
                    int wordPos = *(*q)->wordPos.get(entry->m_word);
                    if (wordCounterArray[get_element_from_2d(arrayWidth, qid, wordPos)] == 0) {
                        wordCounterArray[get_element_from_2d(arrayWidth, qid, wordPos)] = 1;
                        wordCounterArray[get_element_from_2d(arrayWidth, qid, 5)] += 1;
                    }
                }
            }
        }
    }

    pthread_mutex_unlock(&app.word_counter_mtx);

    delete[] hammingArray;

    return true;
}

//---------------------------------------------------------
//   C Interface
//---------------------------------------------------------

ErrorCode InitializeIndex()
{
    return app.initialize();
}

ErrorCode DestroyIndex()
{
    return app.destroy();
}

ErrorCode StartQuery(QueryID query_id, const char* query_str, MatchType match_type, unsigned int match_dist)
{
    Query* query = new Query(query_id, query_str, match_type, match_dist);
#ifdef DOCUMENT_THREADING
    StartJob* newJob = new StartJob(query);

    return app.submitJob(newJob);
#elif WORD_THREADING
    return app.start(query);
#endif
}

// not worth the overhead of creating a job and assigning it to a thread
ErrorCode EndQuery (QueryID query_id)
{
    return app.end(query_id);
}

ErrorCode MatchDocument(DocID doc_id, const char* doc_str)
{
#ifdef DOCUMENT_THREADING
    MatchDocumentJob* newJob = new MatchDocumentJob(doc_id, strdup(doc_str));

    return app.submitJob(newJob);
#elif WORD_THREADING
    return app.match(doc_id, doc_str);
#endif
}

// can't be a job so the program blocks here neccessarily
ErrorCode GetNextAvailRes(DocID* p_doc_id, unsigned int* p_num_res, QueryID** p_query_ids)
{
    DocResult* result;

    auto e = app.getResult(result);

    *p_doc_id = result->id;
    *p_num_res = result->queries.count();
    *p_query_ids = new QueryID[result->queries.count()];

    int i = 0;
    for (auto& x : result->queries) {
        ((*p_query_ids)[i]) = x;
        i++;
    }

    delete result;

    return e;
}
