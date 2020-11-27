//
// Created by Paolo Gastaldi on 26/11/2020.
//

#include "Comm_error.h"

Comm_error::Comm_error(COMM_ERRNO comm_errno, std::string &&location, std::string &&message) : comm_errno(comm_errno), location(std::move(location)), message(std::move(message)) {}