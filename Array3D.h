#ifndef LFA_TEMA_3_ARRAY3D_H
#define LFA_TEMA_3_ARRAY3D_H

#include <vector>
#include <stdexcept>

template<typename T>
class Array3D {
    std::size_t big_size, middle_size, little_size;
    std::vector<T> array;
public:
    Array3D(std::size_t b_size, std::size_t m_size, std::size_t l_size, const T& init);
    std::vector<T>::reference at(std::size_t b_index, std::size_t m_index, std::size_t l_index);
};

#include "Array3D.tpp"

#endif //LFA_TEMA_3_ARRAY3D_H
