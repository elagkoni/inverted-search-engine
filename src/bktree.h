#ifndef BKTREE_H
#define BKTREE_H

#include "list.h"
#include "entry.h"
#include "utilities.h"


// TODO: move to .cpp
struct BKTreeEntry
{
    BKTreeEntry(Entry* entry);
    ~BKTreeEntry();

    void print(int indentation) const;

    Entry*       m_entry                            { nullptr }; /* doesn't own this */
    BKTreeEntry* m_children[MAX_WORD_LENGTH + 1]    { nullptr }; /* initialize all to NULL, BKTreeEntry owns these */
};

class BKTree
{
public:
    BKTree(Matching distanceFunc);
    BKTree(EntryList& list, Matching distanceFunc);
    ~BKTree();

    void insert(Entry* entry);
    EntryList* lookup(const char* word, const unsigned int maxDistance) const;
    void lookup(const char* word, const unsigned int maxDistance, EntryList& list) const;
    EntryList* multiLookup(const char* word, unsigned int minDistance, unsigned int maxDistance) const;
    void print() const;

private:
    BKTreeEntry *m_root         { nullptr }; /* owns this, which in turn owns its children */
    Matching     m_distanceFunc { nullptr };
};

#endif // BKTREE_H
