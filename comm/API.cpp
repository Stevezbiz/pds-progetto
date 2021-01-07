//
// Created by stevezbiz on 26/11/20.
//

#include "API.h"

#include <utility>

API::API(Socket_API *socket_api) : api_(socket_api) {}

bool API::save_file_(const std::shared_ptr<Message> &message) {
    Utils::write_on_file(boost::filesystem::path{ message->path }, message->file);
    return true;
}

std::shared_ptr<Message> API::get_message() {
    return api_->get_message();
}

std::shared_ptr<Comm_error> API::get_last_error() {
    return api_->get_last_error();
}

API::~API() {
    delete api_;
}
