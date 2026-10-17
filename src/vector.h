#ifndef MYVECTOR_H
#define MYVECTOR_H

#include <iostream>
#include <cstddef>


//---------------------------------------------------------
//   Vector
//---------------------------------------------------------

template<typename T>
class Vector
{
public:
    Vector();
    Vector(size_t size);
    Vector(T* array, size_t size);
    ~Vector();

    T at(unsigned int index);

    void pushBack(T data);
    void insert(unsigned int index, T data);

    unsigned int count() const;
    size_t size() const;

private:
    void doubleSize();

    T*              m_array { nullptr };
    unsigned int    m_count { 0 };
    size_t          m_size  { 0 };
};

#include "vector_impl.h"

#endif // MYVECTOR_H
