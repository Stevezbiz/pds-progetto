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

std::string Session::get_user() {
    return user_;
}

bool Session::create_file(const std::string &path, const std::string &hash) {
    return files_.insert(std::make_pair(path,hash)).second;
}

bool Session::modify_file(const std::string &path, const std::string &hash) {
    if(files_contains(hash)) {
        files_[path] = hash;
        return true;
    }
    return false;
}

bool Session::remove_file(const std::string &path) {
    return files_.erase(path);
}

bool Session::files_contains(const std::string &key) {
    auto el = files_.find(key);
    return el != files_.end();
}

const std::vector<std::string> *Session::get_paths() {
    std::vector<std::string> paths;
    paths.reserve(files_.size());
    for(const auto& it : files_){
        paths.push_back(it.first);
        paths.emplace_back(it.first);
    }
    return new std::vector<std::string>(std::move(paths));
}
