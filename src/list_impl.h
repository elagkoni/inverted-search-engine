#ifndef LIST_IMPL_H
#define LIST_IMPL_H

#include <stdexcept>
#include <assert.h>
#include "list.h"

//---------------------------------------------------------
//   ListNode
//---------------------------------------------------------

template<typename T>
ListNode<T>::ListNode(T data)
{
    m_data = new T;
    *m_data = data;
}

template<typename T>
ListNode<T>::~ListNode()
{
    if (m_data) {
        delete m_data;
    }
}

//---------------------------------------------------------
//   LinkedList
//---------------------------------------------------------

template<typename T>
LinkedList<T>::LinkedList()
{
    m_head = new ListNode<T>();
    m_tail = new ListNode<T>();
    m_head->m_next = m_tail;
    m_tail->m_previous = m_head;
    m_head->m_dummy = true;
    m_tail->m_dummy = true;
}

template<typename T>
LinkedList<T>::~LinkedList()
{
    auto node = m_head->m_next;
    while (node != m_tail) {
        auto temp = node;
        node = node->m_next;
        delete temp;
    }
    delete m_head;
    delete m_tail;
}

template<typename T>
unsigned int LinkedList<T>::count() const
{
    return m_count;
}

template<typename T>
ListNode<T>* LinkedList<T>::nodeAt(unsigned int index) const
{
    if (index >= m_count) {
        return nullptr;
    }

    ListNode<T>* node = m_head->m_next;
    for (unsigned int i = 0; i < index; i++) {
        node = node->m_next;
    }
    return node;
}

template<typename T>
T LinkedList<T>::at(unsigned int index) const
{
    return *(nodeAt(index)->m_data);
}

template<typename T>
T LinkedList<T>::first() const
{
    return at(0);
}

template<typename T>
T LinkedList<T>::last() const
{
    return at(m_count - 1);
}

template<typename T>
ListNode<T>* LinkedList<T>::head() const
{
    return m_head;
}

template<typename T>
ListNode<T>* LinkedList<T>::tail() const
{
    return m_tail;
}

template<typename T>
void LinkedList<T>::remove(ListNode<T>* node)
{
    node->m_previous->m_next = node->m_next;
    node->m_next->m_previous = node->m_previous;
    delete node;
    m_count--;
}

template<typename T>
void LinkedList<T>::remove(unsigned int index)
{
    ListNode<T>* node = LinkedList<T>::nodeAt(index);
    remove(node);
}

template<typename T>
void LinkedList<T>::remove(unsigned int bIndex, unsigned int eIndex)
{
    unsigned int i = bIndex;

    for (unsigned int count = 0; count <= eIndex - bIndex; count++) {
        remove(i);
    }
}

template<typename T>
void LinkedList<T>::clear()
{
    while (m_count > 0) {
        remove((int)0);
    }
}

template<typename T>
ListIterator<T> LinkedList<T>::begin() const
{
    return ListIterator<T>(m_head->m_next);
}

template<typename T>
ListIterator<T> LinkedList<T>::end() const
{
    return ListIterator<T>(m_tail);
}

//---------------------------------------------------------
//   List
//---------------------------------------------------------

template<typename T>
ListNode<T>* List<T>::insert(T data, unsigned int index)
{
    ListNode<T>* node = index == 0 ? (this->m_head) : LinkedList<T>::nodeAt(index - 1); // node before nodeAt(index)
    ListNode<T>* newNode = new ListNode<T>(data);
    node->m_next->m_previous = newNode;
    newNode->m_previous = node;
    newNode->m_next = node->m_next;
    node->m_next = newNode;
    (this->m_count)++;

    return newNode;
}

template<typename T>
ListNode<T>* List<T>::append(T data)
{
    ListNode<T>* newNode = new ListNode<T>(data);
    this->m_tail->m_previous->m_next = newNode;
    newNode->m_previous = this->m_tail->m_previous;
    newNode->m_next = this->m_tail;
    this->m_tail->m_previous = newNode;
    (this->m_count)++;

    return newNode;
}

//---------------------------------------------------------
//   OrderedList
//---------------------------------------------------------

template<typename T>
ListNode<T>* OrderedList<T>::insert(T data)
{
    ListNode<T>* newNode = new ListNode<T>(data);
    ListNode<T>* node = this->m_head;
    while (node->m_next != this->m_tail) {
        node = node->m_next;
        ListNode<T>* dataNode = node;
        if (m_compare(*(dataNode->m_data), data) == 1) {
            node = node->m_previous;
            break;
        }
        if (node->m_next == this->m_tail) {
            break;
        }
    }

    node->m_next->m_previous = (newNode);
    newNode->m_next = (node->m_next);
    newNode->m_previous = node;
    node->m_next = newNode;
    (this->m_count)++;

    return newNode;
}

#endif // LIST_IMPL_H
