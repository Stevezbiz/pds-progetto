//
// Created by Paolo Gastaldi on 01/12/2020.
//

#ifndef TEST_STATIC_LOGGER_H
#define TEST_STATIC_LOGGER_H

#include <iostream>
#include "Comm_error.h"

enum LOG_CODE : int {
    LOG_INFO = 0,
    LOG_WARNING = 1,
    LOG_ERROR = 2
};

enum PRIORITY : int {
    PR_VERY_LOW = 0,
    PR_LOW = 1,
    PR_NORMAL = 2,
    PR_HIGH = 3,
    PR_VERY_HIGH = 4
};

class Logger {

public:
    /**
     * create a new log
     * @param log_code
     * @param location
     * @param message
     * @param priority
     */
    static void log(LOG_CODE log_code, const std::string &location, const std::string &message, PRIORITY priority = PR_HIGH) {
        std::cout << "[Log code " + std::to_string(log_code) + "] " + message +  " (" + location + ") (priority " << priority << ")" << std::endl;
    }

    /**
     * create a new info log
     * @param location
     * @param message
     * @param priority
     */
    static void info(const std::string &location, const std::string &message, PRIORITY priority = PR_HIGH) {
        Logger::log(LOG_INFO, location, message, priority);
    }

    /**
     * create a new warning log
     * @param location
     * @param message
     * @param priority
     */
    static void warning(const std::string &location, const std::string &message, PRIORITY priority = PR_HIGH) {
        Logger::log(LOG_WARNING, location, message, priority);
    }

    /**
     * create a new error log
     * @param location
     * @param message
     * @param priority
     */
    static void error(const std::string &location, const std::string &message, PRIORITY priority = PR_HIGH) {
        Logger::log(LOG_ERROR, location, message, priority);
    }

    /**
     * create a new error log
     * @param comm_error
     * @param priority
     */
    /*static void error(Comm_error *comm_error, PRIORITY priority = PR_HIGH) {
        Logger::log(LOG_ERROR, comm_error->location, comm_error->to_string(), priority);
    }*/
};


#endif //TEST_STATIC_LOGGER_H
