#ifndef LFA_TEMA_3_ARRAY3D_TPP
#define LFA_TEMA_3_ARRAY3D_TPP

#ifndef LFA_TEMA_3_ARRAY3D_H
#error "Array3D.tpp" should be included only in "Array3D.h"!
#endif

template <typename T>
Array3D<T>::Array3D(std::size_t b_size, std::size_t m_size, std::size_t l_size, const T& init)
: big_size(b_size), middle_size(m_size), little_size(l_size), array(big_size * middle_size * little_size, init) {}

template <typename T>
std::vector<T>::reference Array3D<T>::at(std::size_t b_index, std::size_t m_index, std::size_t l_index) {
    if(b_index >= big_size) throw std::invalid_argument("b_index out of range");
    if(m_index >= middle_size) throw std::invalid_argument("m_index out of range");
    if(l_index >= little_size) throw std::invalid_argument("l_index out of range");
    std::size_t index = (middle_size * little_size) * b_index + (little_size) * m_index + l_index;
    return array.at(index);
}

#endif //LFA_TEMA_3_ARRAY3D_TPP
