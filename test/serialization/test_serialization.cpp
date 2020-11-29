//
// Created by Paolo Gastaldi on 29/11/2020.
//

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

    Inner() : Inner(0) {}
    explicit Inner(int val) : val(val) {}
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

    Outer() : Outer(0, new Inner{ 0 }) {}
    Outer(int val, Inner *inner) : val(val), inner(inner) {}

    ~Outer() {
        delete this->inner;
    };
};

int main(int argc, char **argv) {
    auto inner = new Inner{ 42 };
    auto outer = new Outer{ 73, inner };

    std::ostringstream ostream{};
    boost::archive::text_oarchive oa{ ostream };
    oa << outer;
    auto serialized = ostream.str();

    // other stuffs...

    std::istringstream ss{ serialized };
    boost::archive::text_iarchive ia{ ss };
    Outer *new_outer;
    ia >> new_outer;

    delete outer;
}