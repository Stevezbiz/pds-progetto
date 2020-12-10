//
// Created by Paolo Gastaldi on 09/12/2020.
//

#include "Base.h"

template class Base<int>;
//template class Base<float>;

template<class T>
Base<T>::Base(T val) : val(val) {}

template<class T>
void Base<T>::print(const std::string &location) {
    std::cout << location + ", val = " + std::to_string(this->val) << std::endl;
}