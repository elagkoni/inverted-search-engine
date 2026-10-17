#include "bktree.h"
#include <iostream>
#include <cstring>


//---------------------------------------------------------
//   BKTreeEntry
//---------------------------------------------------------

BKTreeEntry::BKTreeEntry(Entry *entry)
{
    m_entry = entry;
}

BKTreeEntry::~BKTreeEntry()
{
    for (int i = 0; i < MAX_WORD_LENGTH + 1; i++) {
        if (m_children[i]) {
            delete m_children[i];
        }
    }
}

void BKTreeEntry::print(int indentation) const
{
    char* spaces = new char[indentation + 1];
    memset(spaces, ' ', sizeof (char) * indentation);
    spaces[indentation] = '\0';

    std::cout << spaces << m_entry->m_word << std::endl;
    for (int i = 0; i < MAX_WORD_LENGTH + 1; i++) {
        if (m_children[i]) {
            std::cout << spaces << " " << i << ":" << std::endl;
            m_children[i]->print(indentation + 2);
        }
    }

    delete[] spaces;
}

//---------------------------------------------------------
//   BKTree
//---------------------------------------------------------

BKTree::BKTree(Matching distanceFunc)
{
    m_distanceFunc = distanceFunc;
}

BKTree::BKTree(EntryList& list, Matching distanceFunc)
{
    m_distanceFunc = distanceFunc;

    for (auto& entry : list) {
        insert(entry);
    }
}

BKTree::~BKTree()
{
    if (m_root) {
        delete m_root;
    }
}

EntryList* BKTree::lookup(const char *word, const unsigned int maxDistance) const
{
    EntryList*          returnList = new EntryList;
    List<BKTreeEntry*>  candidates {}; /* FIFO */

    auto floor = [](int i) { return i < 1 ? 1 : i; };
    auto ceil  = [](int i) { return i > MAX_WORD_LENGTH ? MAX_WORD_LENGTH : i; };

    if (!m_root) {
        return returnList;
    }

    candidates.append(m_root);

    while (candidates.count() > 0) {
        /* pop first */
        BKTreeEntry* entry = candidates.at(0);
        candidates.remove(static_cast<unsigned int>(0)); /* cast to avoid ambiguity */
        /* calculate distance:
         * if <= than maxDistance then this entry is accepted
         * add [distance - maxDistance, distance + maxDistance] to candidates
         */
        const unsigned int distance = m_distanceFunc(entry->m_entry->m_word, word);
        if (distance <= maxDistance) {
            returnList->append(entry->m_entry);
        }
        for (int i = floor(distance - maxDistance); i <= ceil(distance + maxDistance); i++) {
            if (entry->m_children[i]) {
                candidates.append(entry->m_children[i]);
            }
        }
    }
    return returnList;
}

void BKTree::lookup(const char* word, const unsigned int maxDistance, EntryList& list) const
{
    List<BKTreeEntry*>  candidates {}; /* FIFO */

    auto floor = [](int i) { return i < 1 ? 1 : i; };
    auto ceil  = [](int i) { return i > MAX_WORD_LENGTH ? MAX_WORD_LENGTH : i; };

    if (!m_root) {
        return;
    }

    candidates.append(m_root);

    while (candidates.count() > 0) {
        /* pop first */
        BKTreeEntry* entry = candidates.at(0);
        candidates.remove(static_cast<unsigned int>(0)); /* cast to avoid ambiguity */
        /* calculate distance:
         * if <= than maxDistance then this entry is accepted
         * add [distance - maxDistance, distance + maxDistance] to candidates
         */
        const unsigned int distance = m_distanceFunc(entry->m_entry->m_word, word);
        if (distance <= maxDistance) {
            list.append(entry->m_entry);
        }
        for (int i = floor(distance - maxDistance); i <= ceil(distance + maxDistance); i++) {
            if (entry->m_children[i]) {
                candidates.append(entry->m_children[i]);
            }
        }
    }
}

//EntryList* BKTree::multiLookup(const char* word, unsigned int minDistance, unsigned int maxDistance) const
//{
//    struct Candidate {
//        Candidate(BKTreeEntry* entry, unsigned int dist) : treeEntry(entry), distance(dist) {};
//        BKTreeEntry* treeEntry;
//        unsigned int distance;
//    };

//    EntryList*          returnList = new EntryList[maxDistance - minDistance + 1];
//    List<Candidate*>    candidates {}; /* FIFO */

//    auto floor = [](int i) { return i < 1 ? 1 : i; };
//    auto ceil  = [](int i) { return i > MAX_WORD_LENGTH ? MAX_WORD_LENGTH : i; };

//    if (!m_root) {
//        return returnList;
//    }

//    candidates.append(new Candidate(m_root, minDistance));

//    while (candidates.count() > 0) {
//        /* pop first */
//        Candidate* candidate = candidates.at(0);
//        candidates.remove(static_cast<unsigned int>(0)); /* cast to avoid ambiguity */

//        /* loop from the distance required to reach this candidate
//         * to the maxDistance
//         */
//        bool added { false };
//        for (unsigned int curMaxDistance = minDistance; curMaxDistance <= maxDistance; curMaxDistance++) {
//            /* calculate distance:
//             * if <= than maxDistance then this entry is accepted
//             * add [distance - maxDistance, distance + maxDistance] to candidates
//             */
//            unsigned int otherDistance = curMaxDistance < candidate->distance ? candidate->distance : curMaxDistance;
//            unsigned int distance = m_distanceFunc(candidate->treeEntry->m_entry->m_word, word);
//            if (!added && distance <= otherDistance) {
//                returnList[otherDistance - 1].append(candidate->treeEntry->m_entry);
//                added = true;
//            }

//            for (int i = floor(distance - curMaxDistance); i < floor((distance - (curMaxDistance - 1))); i++) {
//                if (candidate->treeEntry->m_children[i]) {
//                    candidates.append(new Candidate(candidate->treeEntry->m_children[i], otherDistance));
//                }
//            }

//            for (int i = ceil((distance + (curMaxDistance - 1))); i <= ceil(distance + curMaxDistance); i++) {
//                if (candidate->treeEntry->m_children[i]) {
//                    candidates.append(new Candidate(candidate->treeEntry->m_children[i], otherDistance));
//                }
//            }
//        }
//        delete candidate;
//    }

//    return returnList;
//}

// Faster than the "smart" multilookup.
EntryList* BKTree::multiLookup(const char* word, unsigned int minDistance, unsigned int maxDistance) const
{
    EntryList* returnList = new EntryList[maxDistance - minDistance + 1];
    for (unsigned int i = 0; i < maxDistance - minDistance + 1; i++) {
        lookup(word, minDistance + i, returnList[i]);
    }
    return returnList;
}

void BKTree::print() const
{
    if (m_root)
        m_root->print(0);
}

void BKTree::insert(Entry *entry)
{
    BKTreeEntry* newEntry = new BKTreeEntry(entry);

    if (!m_root) {
        m_root = newEntry;
        return;
    }

    BKTreeEntry* current = m_root;
    unsigned int distance = m_distanceFunc(entry->m_word, current->m_entry->m_word);
    while (current->m_children[distance] != nullptr) {
        current = current->m_children[distance];
        distance = m_distanceFunc(entry->m_word, current->m_entry->m_word);
    }
    current->m_children[distance] = newEntry;
}
