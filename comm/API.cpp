//
// Created by stevezbiz on 26/11/20.
//

#include "API.h"

#include <utility>

API::API(std::unique_ptr<Socket_API> socket_api) : api_(std::move(socket_api)) {}

bool API::save_file_(const std::shared_ptr<Message> &message) {
    Utils::write_on_file(fs::path{ message->path }, message->file);
    return true;
}

std::shared_ptr<Message> API::get_message() {
    return this->api_->get_message();
}

std::shared_ptr<Comm_error> API::get_last_error() {
    return this->api_->get_last_error();
}