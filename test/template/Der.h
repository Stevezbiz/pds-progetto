//
// Created by Paolo Gastaldi on 09/12/2020.
//

#ifndef TEST_TEMPLATE_DER_H
#define TEST_TEMPLATE_DER_H

#include "Base.h"

class Der : public Base<int> { // derived class
public:
    explicit Der(int val);
    void foo(int new_val);
    ~Der() = default;
};


#endif //TEST_TEMPLATE_DER_H
