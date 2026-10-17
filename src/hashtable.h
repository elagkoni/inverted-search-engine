#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <iostream>
#include <cstddef>
#include "string.h"
#include "list.h"
#include "utilities.h"


//---------------------------------------------------------
//   Hash Functions
//---------------------------------------------------------

//
// HashFunctions for common data types, the modulo of the return value is used
//
template<typename KEY> int HashFunction(KEY);
template<typename KEY> int HashFunction(const char* s);
template<typename KEY> int HashFunction(unsigned int i);

//---------------------------------------------------------
//   HashTableNode
//---------------------------------------------------------

template<typename KEY, typename DATA>
struct HashtableNode
{
    HashtableNode() {}
    HashtableNode(KEY k, DATA d);
    ~HashtableNode();

    KEY  *key  { nullptr };
    DATA *data { nullptr };
};

//---------------------------------------------------------
//   HashTable
//---------------------------------------------------------

template<typename KEY, typename DATA>
class Hashtable
{
public:
    Hashtable(size_t size, Equal<KEY> equalsFunc);
    ~Hashtable();

    int hash(KEY k) const;
    void insert(KEY k, DATA d);
    DATA* get(KEY k) const;
    void clear();
    List<KEY>& keys();

private:
    void rehash();
    void deleteDataArray(List<HashtableNode<KEY, DATA>*>* dataArray, size_t dataArraySize);

    List<HashtableNode<KEY, DATA>*>* m_dataArray         { nullptr }; // array of lists, use pointers to avoid constructors/destructors
    List<KEY>                        m_keys;
    size_t                           m_dataArraySize     { 0 };
    Equal<KEY>                       m_equals            { nullptr };

    static const unsigned int        REHASHING_THRESHOLD { 4 };
};

#include "hashtable_impl.h"

#endif // HASHTABLE_H
