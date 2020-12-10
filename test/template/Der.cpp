//
// Created by Paolo Gastaldi on 09/12/2020.
//

#include "Der.h"

Der::Der(int val) : Base(val) {};

void Der::foo(int new_val) {
    this->val = new_val;
    this->print("Der::foo");
}