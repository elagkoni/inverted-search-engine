#ifndef LIST_H
#define LIST_H

#include <iostream>
#include "utilities.h"


//---------------------------------------------------------
//   Nodes
///     ListNode: Your normal doubly linked list node.
//---------------------------------------------------------

template<typename T>
class ListNode
{
public:
    ListNode    () {}
    ListNode    (T data);
    ~ListNode   ();

    T*          m_data      { nullptr };
    ListNode*   m_next      { nullptr };
    ListNode*   m_previous  { nullptr };
    bool        m_dummy     { false };
};

//---------------------------------------------------------
//   Lists
///     LinkedList: Base List class, all lists are derived from
///     this.
///     List: Unordered list.
///     OrderedList: Ordered list.
//---------------------------------------------------------

template<typename T>
class ListIterator
{
public:
    ListIterator(ListNode<T>* node) : m_node(node) {}
    ListIterator operator++()
    {
        m_node = m_node->m_next;
        return *this;
    }
    bool operator!= (const ListIterator& other) const
    {
        return m_node != other.m_node;
    }
    const T& operator* () const
    {
        return *(m_node->m_data);
    }
    T& operator* ()
    {
        return *(m_node->m_data);
    }

private:
    ListNode<T>* m_node { nullptr };
};

template<typename T>
class LinkedList
{
public:
    LinkedList();
    ~LinkedList();

    unsigned int count() const;

    // High Level Interface
    T at(unsigned int index) const;
    T first() const;
    T last() const;
    void remove(unsigned int index);
    void remove(unsigned int bIndex, unsigned int eIndex);
    void clear();

    // Low Level Interface
    ListNode<T>* head() const;
    ListNode<T>* tail() const;
    ListNode<T>* nodeAt(unsigned int index) const;
    void remove(ListNode<T>* node);

    ListIterator<T> begin() const;
    ListIterator<T> end() const;

protected:
    unsigned int m_count { 0 };
    ListNode<T>* m_head;
    ListNode<T>* m_tail;
    Equal<T>     m_equals { nullptr };
};

template<typename T>
class List : public LinkedList<T>
{
public:
    ListNode<T>* insert(T data, unsigned int index);
    ListNode<T>* append(T data);
};

template<typename T>
class OrderedList : public LinkedList<T>
{
public:
    OrderedList(Compare<T> f) { m_compare = f; }

    ListNode<T>* insert(T data);

    Compare<T> m_compare { nullptr };
};

#include "list_impl.h"

#endif // LIST_H
