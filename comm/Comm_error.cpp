//
// Created by Paolo Gastaldi on 26/11/2020.
//

#include "Comm_error.h"

template<class Archive>
void Comm_error::serialize(Archive &ar, const unsigned int version) {
    ar & this->comm_errno;
    ar & this->message;
    ar & this->location;
}

Comm_error::Comm_error(COMM_ERRNO comm_errno, std::string location, std::string message) : comm_errno(comm_errno), location(std::move(location)), message(std::move(message)) {}

std::string Comm_error::send() const {
    std::ostringstream ostream{};
    boost::archive::text_oarchive oa{ ostream };

    // serialize message content
    oa << this;
    return ostream.str();
}

Comm_error *Comm_error::build(const std::string &serialized) {
    std::istringstream istream{ serialized };
    boost::archive::text_iarchive ia{ istream };

    Comm_error *comm_error;
    ia >> comm_error;

    return comm_error;
}

std::string Comm_error::to_string() const {
    return "[Error code " + std::to_string(this->comm_errno) + "] " + this->message +  " (" + this->location + ")";
}
