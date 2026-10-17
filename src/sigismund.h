#ifndef SIGISMUND_H
#define SIGISMUND_H

#include "hashtable.h"
#include "vector.h"
#include "entry.h"
#include "bktree.h"
#include "job.h"
#include "scheduler.h"
#include "../include/core.h"


struct DocResult
{
    DocID id;
    List<QueryID> queries;
};

struct Query
{
    Query(QueryID queryId, const char* str, MatchType matchType, unsigned int matchDist)
        : wordPos(5, equalStrings)
    {
        id = queryId;
        this->matchType = matchType;
        this->matchDist = matchDist;
        wordsNum = 0;

        char* copy = strdup(str);
        char* rest;
        char* token = strtok_r(copy, " ", &rest);

        while(token != NULL) {
            if (wordPos.get(token) == nullptr) {
                wordPos.insert(strdup(token), wordsNum);
                wordsNum++;
            }

            /* get next word */
            token = strtok_r(NULL, " ", &rest);
        }

        strcpy(word, str);
        free(copy);
    };

    ~Query() {
        for (auto& x : wordPos.keys()) {
            free((void*) x);
        }
    }

    QueryID id;
    MatchType matchType;
    unsigned int matchDist;

    char word[MAX_QUERY_LENGTH];

    Hashtable<const char*, int> wordPos;
    unsigned int wordsNum;    // number of words in the Query
};

//---------------------------------------------------------
//   Sigismund
//---------------------------------------------------------

class Sigismund
{
public:
    Sigismund();

    ErrorCode initialize();
    ErrorCode initialize(int argc, char** argv);
    ErrorCode destroy();
    ErrorCode start(Query*& query);
    ErrorCode end(QueryID queryId);
    ErrorCode match(DocID doc_id, const char* doc_str);
    ErrorCode getResult(DocResult*& d);

    ErrorCode submitJob(Job* job);

private:
    ErrorCode initializeShared();

    ErrorCode addWordToHashtable(const char *word, QueryID id, Hashtable<const char*, Entry*>*& table, Entry *&out_entry);
    ErrorCode insertQueryExact(Query*& query);
    ErrorCode insertQueryHamming(Query*& query);
    ErrorCode insertQueryEdit(Query*& query);

    /* job handling */
    Scheduler*                      jobSched                        { nullptr };

    /* used to delete Entry instances */
    EntryList                       allEntries;

    /* indices */
    Hashtable<const char*, Entry*>* exactIndex                     { nullptr };
    BKTree*                         editIndex                      { nullptr };
    BKTree* hammingIndex[MAX_WORD_LENGTH - MIN_WORD_LENGTH + 1]    { nullptr };

    /* if queries are guaranteed to be given in order then we can use an array */
    Hashtable<QueryID, Query*>*     queries                        { nullptr };
    Hashtable<QueryID, QueryID>*    inactiveQueries                { nullptr };

    /* results */
    List<DocResult*>                results                        {};
    pthread_mutex_t                 results_mtx;
    sem_t                           results_not_empty;

    /* start */
    pthread_mutex_t                 queries_mtx;
    pthread_mutex_t                 exact_mtx;
    pthread_mutex_t                 edit_mtx;
    pthread_mutex_t                 hamming_mtx[MAX_WORD_LENGTH - MIN_WORD_LENGTH + 1];
    pthread_mutex_t                 entries_mtx;
    pthread_mutex_t                 hamming_entry_mtx;
    pthread_mutex_t                 edit_entry_mtx;

    pthread_mutex_t                 word_counter_mtx;

    /* used for updating the payloads of existing Entries in BKTrees */
    Hashtable<const char*, Entry*>* editEntries                    { nullptr };
    Hashtable<const char*, Entry*>* hammingEntries                 { nullptr };

    friend class MatchWordJob;
};

//---------------------------------------------------------
//   Jobs
//---------------------------------------------------------

class StartJob : public Job
{
public:
    StartJob(Query* query);
    ~StartJob();

    bool execute() override;

private:
    Query* m_query;
};

class MatchDocumentJob : public Job
{
public:
    MatchDocumentJob(DocID docId, char* docStr);
    ~MatchDocumentJob();

    bool execute() override;

private:
    DocID m_docId;
    char* m_docStr;
};

class MatchWordJob : public Job
{
public:
    MatchWordJob(const char* word, unsigned int*& wordCounterArray, size_t arrayWidth);
    ~MatchWordJob();

    bool execute() override;

private:
    const char* word;
    unsigned int*& wordCounterArray;
    size_t arrayWidth;
};

#endif // SIGISMUND_H
