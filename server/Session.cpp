//
// Created by stevezbiz on 07/11/20.
//

#include "Session.h"

#include <utility>

using namespace boost::asio;

Session::Session(ip::tcp::socket socket, Database_API &database) : api_(std::move(socket)), database_(database) {}

void Session::set_user(std::string user) {
    this->user_ = std::move(user);
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

std::string Session::get_user() {
    return user_;
}

bool Session::create_file(const std::string &path, const std::string &hash) {
    return database_.insert_path(path,hash,user_);
}

bool Session::modify_file(const std::string &path, const std::string &hash) {
    return database_.update_path(path,hash);
}

bool Session::remove_file(const std::string &path) {
    return database_.delete_path(path);
}

const std::vector<std::string> *Session::get_paths() {
    return database_.get_path_schema(user_);
}
