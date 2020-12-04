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
