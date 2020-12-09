//
// Created by Paolo Gastaldi on 09/12/2020.
//

#ifndef TEST_TEMPLATE_SERVICE_H
#define TEST_TEMPLATE_SERVICE_H

#include "Der.h"

class Service {
public:
    Der *der;
    explicit Service(int val);
    void bar() const;
    ~Service();
};


#endif //TEST_TEMPLATE_SERVICE_H
