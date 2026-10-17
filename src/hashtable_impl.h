#ifndef HASHTABLE_IMPL_H
#define HASHTABLE_IMPL_H

#include "hashtable.h"

//---------------------------------------------------------
//   Hash Functions
//---------------------------------------------------------

template<typename KEY>
int HashFunction(const char* s)
{
    return djb2(s);
}

template<typename KEY>
int HashFunction(unsigned int i)
{
    return i;
}

//---------------------------------------------------------
//   HashTableNode
//---------------------------------------------------------

template<typename KEY, typename DATA>
HashtableNode<KEY, DATA>::HashtableNode(KEY k, DATA d)
{
    key = new KEY;
    *key = k;
    data = new DATA;
    *data = d;
}

template<typename KEY, typename DATA>
HashtableNode<KEY, DATA>::~HashtableNode()
{
    delete key;
    delete data;
}

//---------------------------------------------------------
//   HashTable
//---------------------------------------------------------

template<typename KEY, typename DATA>
Hashtable<KEY, DATA>::Hashtable(size_t size, Equal<KEY> equalsFunc)
{
    m_dataArray = new List<HashtableNode<KEY, DATA>*>[size];
    m_dataArraySize = size;
    m_equals = equalsFunc;
}

template<typename KEY, typename DATA>
Hashtable<KEY, DATA>::~Hashtable()
{
    deleteDataArray(m_dataArray, m_dataArraySize);
}

template<typename KEY, typename DATA>
void Hashtable<KEY, DATA>::deleteDataArray(List<HashtableNode<KEY, DATA> *> *dataArray, size_t dataArraySize)
{
    for (unsigned int i = 0; i < dataArraySize; i++) {
        auto& list = dataArray[i]; // reference because the lists must be deleted later
        if (list.count() == 0) {
            continue;
        }
        for (auto& hashNode : list) {
            delete hashNode;
        }
    }

    delete[] dataArray;
}

template<typename KEY, typename DATA>
int Hashtable<KEY, DATA>::hash(KEY k) const
{
    return HashFunction<KEY>(k) % m_dataArraySize;
}

template<typename KEY, typename DATA>
void Hashtable<KEY, DATA>::insert(KEY k, DATA d)
{
    int index = hash(k);
    auto& list = m_dataArray[index];

    // Seems to slow us down.
    // if we are about to exceed the rehashing threshold rehash and then try
    // to insert again
    if (list.count() == REHASHING_THRESHOLD) {
        rehash();
        insert(k, d);
        return;
    }

    // do not allow duplicates, not neccessary in our use case
    for (auto& x : list) {
        if (m_equals(*(x->key), k)) {
            return;
        }
    }

    list.append(new HashtableNode<KEY, DATA>(k, d));
    m_keys.append(k);
}

template<typename KEY, typename DATA>
DATA* Hashtable<KEY, DATA>::get(KEY k) const
{
    int index = hash(k);
    for (auto& node : m_dataArray[index]) {
        if (m_equals(*node->key, k)) {
            return node->data;
        }
    }
    return nullptr;
}

template<typename KEY, typename DATA>
void Hashtable<KEY, DATA>::clear()
{
    for (int i = 0; i < m_dataArraySize; i++) {
        auto& list = m_dataArray[i]; // reference because the lists must be deleted later
        if (list.count() == 0) {
            continue;
        }
        for (auto& hashNode : list) {
            delete hashNode;
        }
        list.clear();
    }
    m_keys.clear();
}

template<typename KEY, typename DATA>
List<KEY>& Hashtable<KEY, DATA>::keys()
{
    return m_keys;
}


template<typename KEY, typename DATA>
void Hashtable<KEY, DATA>::rehash()
{
    // hold old array data
    auto oldDataArray = m_dataArray;
    auto oldDataArraySize = m_dataArraySize;

    // allocate new array
    m_dataArray = new List<HashtableNode<KEY, DATA>*>[oldDataArraySize * 2];
    m_dataArraySize = oldDataArraySize * 2;

    // remove existing keys list
    m_keys.clear();

    // insert elements into new array
    for (size_t i = 0; i < oldDataArraySize; i++) {
        auto& list = oldDataArray[i];
        for (auto& x : list) {
            insert(*(x->key), *(x->data));
        }
    }

    // deallocate old array data
    deleteDataArray(oldDataArray, oldDataArraySize);
}

#endif // HASHTABLE_IMPL_H
