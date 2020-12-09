//
// Created by Paolo Gastaldi on 09/12/2020.
//

#include <iostream>

template <class T>
class Base {
public:
    T val;
    explicit Base(T val) : val(val) {};
    void print(const std::string &location = "Base::print") {
        std::cout << location + ", val = " + std::to_string(this->val) << std::endl;
    };
    ~Base() = default;
};

class Der : public Base<int> { // derived class
public:
    explicit Der(int val) : Base(val) {};
    void foo(int new_val) {
        this->val = new_val;
        this->print("Der::foo");
    };
    ~Der() = default;
};

class Service {
public:
    Der *der;
    explicit Service(int val) : der(new Der{ val }) {};
    void bar() {
        this->der->print("Service::bar");
    }
    ~Service() {
        delete this->der;
    }
};

int main(int argc, char **argv) {
    Der der{ 5 };
    der.print();
    der.foo(8);

    auto service = new Service{ 3 };
    service->bar();
    delete service;
}