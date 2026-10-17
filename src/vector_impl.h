#ifndef VECTOR_IMPL_H
#define VECTOR_IMPL_H

#include <cstring>
#include "vector.h"


template<typename T>
Vector<T>::Vector()
{
    m_array = new T[10];
    m_size = 10;
    m_count = 0;

    memset(m_array, 0, sizeof(T) * 10);
}

template<typename T>
Vector<T>::Vector(size_t size)
{
    m_array = new T[size];
    m_size = size;
    m_count = 0;

    memset(m_array, 0, sizeof(T) * m_size);
}

template<typename T>
Vector<T>::Vector(T *array, size_t size)
{
    m_array = array;
    m_size = size;
    m_count = size;
}

template<typename T>
Vector<T>::~Vector()
{
    delete[] m_array;
}

template<typename T>
T Vector<T>::at(unsigned int index)
{
    return m_array[index];
}

template<typename T>
void Vector<T>::pushBack(T data)
{
    // resize if needed
    if (m_count == m_size) {
        doubleSize();
    }

    m_array[m_count] = data;
    m_count++;
}

template<typename T>
void Vector<T>::insert(unsigned int index, T data)
{
    if (index >= m_size || index < 0) {
        throw std::out_of_range("Vector: Out of range");
    }

    m_array[index] = data;
}

template<typename T>
unsigned int Vector<T>::count() const
{
    return m_count;
}

template<typename T>
size_t Vector<T>::size() const
{
    return m_size;
}

template<typename T>
void Vector<T>::doubleSize()
{
    T* oldArray = m_array;

    m_array = new T[m_size * 2];
    memcpy(m_array, oldArray, sizeof(T) * m_size);
    m_size = m_size * 2;

    delete[] oldArray;
}

#endif // VECTOR_IMPL_H
