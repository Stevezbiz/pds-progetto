//
// Created by Paolo Gastaldi on 29/11/2020.
//

#include <iostream>
#include <sstream>
#include <boost/tuple/tuple.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

class Inner {
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & this->val;
    }

    friend class boost::serialization::access;

public:
    int val;

    explicit Inner(int val = 0) : val(val) {}
};

class Outer {
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & this->val;
        ar & this->inner;
    }

    friend class boost::serialization::access;

public:
    int val;
    Inner *inner;

    explicit Outer(int val = 0, Inner *inner = new Inner{ 0 }) : val(val), inner(inner) {}

    ~Outer() {
        delete this->inner;
    };

    std::string send() {
        std::ostringstream ostream{};
        boost::archive::text_oarchive oa{ ostream };
        oa << this;
        return ostream.str();
    }

    static Outer *build(const std::string &serialized) {
        std::istringstream ss{ serialized };
        boost::archive::text_iarchive ia{ ss };

        Outer *new_outer;
        ia >> new_outer;

        return new_outer;
    }
};

int main(int argc, char **argv) {
    auto inner = new Inner{ 42 };
    auto outer = new Outer{ 73, inner };

    // ------------ TEST 1 ------------

    std::ostringstream ostream{};
    boost::archive::text_oarchive oa{ ostream };
    oa << outer;
    auto serialized = ostream.str();

    // other stuffs...

    std::istringstream ss{ serialized };
    boost::archive::text_iarchive ia{ ss };
    Outer *new_outer;
    ia >> new_outer;

    std::cout << new_outer->val << std::endl;
    std::cout << new_outer->inner->val << std::endl;

    // ------------ TEST 2 ------------

    auto serialized2 = outer->send();

    // other stuffs...

    auto new_outer2 = Outer::build(serialized);
    std::cout << new_outer2->val << std::endl;
    std::cout << new_outer2->inner->val << std::endl;

    // ------------ END ------------

    delete outer;
    delete new_outer;
    delete new_outer2;

    return 0;
}