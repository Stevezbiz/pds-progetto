//
// Created by Paolo Gastaldi on 01/12/2020.
//

#include "Logger.h"

PRIORITY Logger::min_priority = PR_NULL;

void Logger::log(LOG_CODE log_code, const std::string &location, const std::string &message, PRIORITY priority) {
    if(priority > Logger::min_priority)
        std::cout << "[Log code " + std::to_string(log_code) + "] " + message +  " (" + location + ") (priority " << priority << ")" << std::endl;
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

/*void Logger::error(const Comm_error *comm_error, PRIORITY priority) { // TODO: error: unknown type name 'Comm_error'
    Logger::error(comm_error->location, comm_error->to_string(), priority);
}*/
