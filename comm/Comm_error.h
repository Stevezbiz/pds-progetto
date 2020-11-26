//
// Created by Paolo Gastaldi on 26/11/2020.
//

#ifndef COMM_ERROR_H
#define COMM_ERROR_H

#include <iostream>

/**
 * list of error numbers (error codes)
 */
enum ERRNO : int {
    GENERIC = 0,
    FAILURE = 1,
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
    ERRNO errno;
    std::string location;
    std::string message;

    /**
     * class constructor
     * @param errno
     * @param location
     * @param message
     */
    Comm_error(ERRNO errno = GENERIC, const std::string &location = "System", const std::string message = "Generic error") : errno(errno), location(location), message(message) {}
};


#endif //COMM_ERROR_H
