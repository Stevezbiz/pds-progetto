//
// Created by Paolo Gastaldi on 09/12/2020.
//

#include "Service.h"

Service::Service(int val) : der(new Der{ val }) {}

void Service::bar() const {
    this->der->print("Service::bar");
}

Service::~Service() {
    delete this->der;
}
