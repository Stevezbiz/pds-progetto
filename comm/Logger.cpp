//
// Created by Paolo Gastaldi on 01/12/2020.
//

#include "Logger.h"

PRIORITY Logger::min_priority = PR_NULL;
std::mutex Logger::m_write_;

enum LOG_LENGTHS : int {
    log_length = 10,
    location_length = 50,
    message_length = 100,
    priority_length = 1
};

void Logger::log(LOG_CODE log_code, const std::string &location, const std::string &message, PRIORITY priority) {
    std::lock_guard lg(Logger::m_write_);

    std::string log_string;
    switch(log_code) {
        case LOG_INFO:
            log_string = "INFO";
            break;
        case LOG_WARNING:
            log_string = "WARNING";
            break;
        case LOG_ERROR:
            log_string = "ERROR";
            break;
        default:
            log_string = "UNDEFINED";
            break;
    }
    auto curr_message = message.length() <= message_length ? message : message.substr(0, message_length-6) + " (...)";

    if(priority > Logger::min_priority) {
        std::cout << std::left << std::setw(log_length) << "[" + log_string + "]" << std::left << std::setw(location_length) << "(" + location + ")" << std::left  << std::setw(message_length) << curr_message << std::setw(priority_length) << "(PR " + std::to_string(priority) + ")" << std::endl;
    }
}

void Logger::info(const std::string &location, const std::string &message, PRIORITY priority) {
    Logger::log(LOG_INFO, location, message, priority);
}

void Logger::warning(const std::string &location, const std::string &message, PRIORITY priority) {
    Logger::log(LOG_WARNING, location, message, priority);
}

void Logger::error(const std::string &location, const std::string &message, PRIORITY priority) {
    Logger::log(LOG_ERROR, location, message, priority);
}

void Logger::error(const Comm_error *comm_error, PRIORITY priority) {
    Logger::error(comm_error->location, comm_error->to_string(), priority);
}
