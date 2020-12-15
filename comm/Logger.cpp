//
// Created by Paolo Gastaldi on 01/12/2020.
//

#include "Logger.h"

PRIORITY Logger::logger_filter = PR_NULL;
std::mutex Logger::m_write_;

enum LOG_LENGTHS : int {
    log_length = 10,
    location_length = 35,
    message_length = 110,
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

    if(priority > Logger::logger_filter) {
        auto curr_location = location.length() <= location_length-2 ? location : location.substr(0, location_length-2-3) + "..."; // -2 (two brackets)
        curr_location.erase(std::remove(curr_location.begin(), curr_location.end(), '\n'), curr_location.end()-1);
        auto curr_message = message.length() <= message_length ? message : message.substr(0, message_length-3) + "...";
        curr_message.erase(std::remove(curr_message.begin(), curr_message.end(), '\n'), curr_message.end());

        std::cout << std::left << std::setw(log_length) << "[" + log_string + "]" << std::left << std::setw(location_length+1) << "(" + curr_location + ")" << std::left  << std::setw(message_length+1) << curr_message << std::setw(priority_length) << "(PR " + std::to_string(priority) + ")" << std::endl;
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
