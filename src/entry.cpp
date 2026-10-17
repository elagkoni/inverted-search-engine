#include "entry.h"
#include <cstring>


//---------------------------------------------------------
//   Entry
//---------------------------------------------------------

Entry::Entry(const char* word)
{
    m_word = strdup(word); /* uses malloc so we need to free, not delete it */
}

Entry::~Entry()
{
    free(m_word);
}
