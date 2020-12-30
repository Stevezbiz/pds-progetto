//
// Created by Paolo Gastaldi on 01/12/2020.
//

#ifndef TEST_STATIC_LOGGER_H
#define TEST_STATIC_LOGGER_H

#include <iostream>
#include <sstream>
#include <memory>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Comm_error.h"

enum LOG_CODE : int {
    LOG_INFO = 0,
    LOG_WARNING = 1,
    LOG_ERROR = 2
};

enum PRIORITY : int {
    PR_NULL = 0,
    PR_VERY_LOW = 1,
    PR_LOW = 2,
    PR_NORMAL = 3,
    PR_HIGH = 4,
    PR_VERY_HIGH = 5,
    PR_TOP = 6
};

class Logger {
    static std::mutex m_write_;

public:
    static PRIORITY logger_filter;
    static std::ostream output_;

    /**
     * create a new log
     * @param log_code
     * @param location
     * @param message
     * @param priority
     * @param timestamp
     */
    static void log(LOG_CODE log_code, const std::string &location, const std::string &message, PRIORITY priority = PR_HIGH, boost::posix_time::ptime timestamp = boost::posix_time::second_clock::local_time());

    /**
     * create a new info log
     * @param location
     * @param message
     * @param priority
     * @param timestamp
     */
    static void info(const std::string &location, const std::string &message, PRIORITY priority = PR_VERY_LOW, boost::posix_time::ptime timestamp = boost::posix_time::second_clock::local_time());
    /**
     * create a new warning log
     * @param location
     * @param message
     * @param priority
     * @param timestamp
     */
    static void warning(const std::string &location, const std::string &message, PRIORITY priority = PR_NORMAL, boost::posix_time::ptime timestamp = boost::posix_time::second_clock::local_time());

    /**
     * create a new error log
     * @param location
     * @param message
     * @param priority
     * @param timestamp
     */
    static void error(const std::string &location, const std::string &message, PRIORITY priority = PR_HIGH, boost::posix_time::ptime timestamp = boost::posix_time::second_clock::local_time());

    /**
     * create a new error log
     * @param comm_error
     * @param priority
     */
    static void error(const Comm_error *comm_error, PRIORITY priority = PR_HIGH);

    /**
     * redirect logs to a new output stream
     * @param new_stream
     */
    static void redirect(std::ostream &new_stream);
};


#endif //TEST_STATIC_LOGGER_H
