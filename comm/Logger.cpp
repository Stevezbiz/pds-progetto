//
// Created by Paolo Gastaldi on 01/12/2020.
//

#include "Logger.h"

PRIORITY Logger::min_priority = PR_NULL;

void Logger::log(LOG_CODE log_code, const std::string &location, const std::string &message, PRIORITY priority) {
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
    if(priority > Logger::min_priority) {
        std::cout << std::left << std::setw(10) << "[" + log_string + "]" << std::left << std::setw(50) << "(" + location + ")" << std::left  << std::setw(100) << message << std::setw(1) << "(PR " + std::to_string(priority) + ")" << std::endl;
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
