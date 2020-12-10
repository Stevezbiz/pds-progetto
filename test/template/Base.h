//
// Created by Paolo Gastaldi on 09/12/2020.
//

#ifndef TEST_TEMPLATE_BASE_H
#define TEST_TEMPLATE_BASE_H

#include <iostream>

template <class T>
class Base {
public:
    T val;
    explicit Base(T val);
    void print(const std::string &location = "Base::print");
    ~Base() = default;
};


#endif //TEST_TEMPLATE_BASE_H
