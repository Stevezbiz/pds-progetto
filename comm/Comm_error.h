//
// Created by Paolo Gastaldi on 26/11/2020.
//

#ifndef COMM_ERROR_H
#define COMM_ERROR_H

#include <iostream>
#include <sstream>
#include <boost/tuple/tuple.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <utility>
#include <boost/asio/error.hpp>
#include <boost/system/error_code.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>

/**
 * list of error numbers (error codes)
 */
enum COMM_ERRNO : int {
    CE_NOT_AN_ERROR = -1,
    CE_GENERIC = 0,
    CE_FAILURE = 1,
    CE_UNEXPECTED_TYPE = 10,
    CE_UNEXPECTED_VALUE = 11,
    CE_WRONG_TYPE = 20,
    CE_WRONG_VALUE = 21,
    CE_NOT_ALLOWED = 30
};

/**
 * standard format for errors
 */
class Comm_error {
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version);

    friend class boost::serialization::access;

public:
    COMM_ERRNO comm_errno;
    std::string location;
    std::string message;
    boost::system::error_code original_ec;
    boost::posix_time::ptime timestamp;

    /**
     * class constructor
     * @param errno
     * @param location
     * @param message
     * @param original_ec
     */
    explicit Comm_error(COMM_ERRNO comm_errno = CE_GENERIC, std::string location = "System", std::string message = "Generic error", boost::system::error_code original_ec = boost::asio::error::try_again);

    /**
     * send an error
     * @return serialized error
     */
    [[nodiscard]] std::string send() const;

    /**
     * build an error from a serialize string
     * @param serialize
     * @return error
     */
    static std::shared_ptr<Comm_error> build(const std::string &serialized);

    /**
     * transform this error to a string
     * @return string
     */
    [[nodiscard]] std::string to_string() const;
};


#endif //COMM_ERROR_H
