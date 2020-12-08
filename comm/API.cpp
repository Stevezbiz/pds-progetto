//
// Created by stevezbiz on 26/11/20.
//

#include "API.h"

API::API(Socket_API *socket_api) : api_(socket_api) {}

bool API::save_file_(Message *message) {
    Utils::write_on_file(fs::path{ message->path }, message->file);
    return true;
}

Message *API::get_message() {
    return this->api_->get_message();
}

Comm_error *API::get_last_error() {
    return this->api_->get_last_error();
}

API::~API() {
    delete this->api_;
}
