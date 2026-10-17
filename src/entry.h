#ifndef ENTRY_H
#define ENTRY_H

#include "list.h"
#include "../include/core.h"


//---------------------------------------------------------
//   Entry
///     m_appearsIn: a list of the indices of the queries where
///                  m_word appears
//---------------------------------------------------------

struct Entry
{
    Entry(const char* word); // m_word is a copy of word, not the original
    ~Entry();

    char*           m_word { nullptr };
    List<QueryID>   m_appearsIn;
};

//---------------------------------------------------------
//   EntryList
///     an alias for LinkedList<Entry*>.
///     using pointers means that deleting the list does not
///     delete the Entries, those need to be deleted explicitly.
//---------------------------------------------------------

using EntryList = List<Entry*>;

#endif // ENTRY_H
