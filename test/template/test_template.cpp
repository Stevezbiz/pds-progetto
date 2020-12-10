//
// Created by Paolo Gastaldi on 09/12/2020.
//

#include <iostream>

#include "Der.h"
#include "Service.h"

int main(int argc, char **argv) {
    Der der{ 5 };
    der.print();
    der.foo(8);

    auto service = new Service{ 3 };
    service->bar();
    delete service;
}