//
// Created by Paolo Gastaldi on 26/11/2020.
//

#ifndef COMM_ERROR_H
#define COMM_ERROR_H

#include <iostream>

/**
 * list of error numbers (error codes)
 */
enum COMM_ERRNO : int {
    NOT_AN_ERROR = -1,
    UNDEFINED = 0,
    GENERIC = 1,
    FAILURE = 2,
    UNEXPECTED_TYPE = 10,
    UNEXPECTED_VALUE = 11,
    WRONG_TYPE = 20,
    WRONG_VALUE = 21
};

/**
 * standard format for errors
 */
class Comm_error {
public:
    COMM_ERRNO comm_errno;
    std::string location;
    std::string message;

    /**
     * class constructor
     * @param errno
     * @param location
     * @param message
     */
    explicit Comm_error(COMM_ERRNO comm_errno = GENERIC, std::string &&location = "System", std::string &&message = "Generic error");
};


#endif //COMM_ERROR_H
