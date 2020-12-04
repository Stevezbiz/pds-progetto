//
// Created by stevezbiz on 07/11/20.
//

#include "Session.h"

#include <utility>

using namespace boost::asio;

Session::Session(ip::tcp::socket socket) : api_(std::move(socket)) {}

void Session::set_user(std::string user) {
    user_ = std::move(user);
}

bool Session::receive(MESSAGE_TYPE expectedMessage) {
    return api_.receive(expectedMessage);
}

Comm_error *Session::get_last_error() {
    return api_.get_last_error();
}

bool Session::send(Message *message) {
    return api_.send(message);
}

Message *Session::get_message() {
    return api_.get_message();
}
