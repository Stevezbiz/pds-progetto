//
// Created by stevezbiz on 26/11/20.
//

#include "API.h"

API::API(Socket_API *socket_api, const std::string &root_path) : api_(socket_api), root_path(root_path) {}

bool API::save_file_(Message *message) {
    fs::path full_path = this->root_path / fs::path{ message->path };
    Utils::write_on_file(full_path, message->file);
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
