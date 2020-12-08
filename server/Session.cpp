//
// Created by stevezbiz on 07/11/20.
//

#include "Session.h"

bool Session::files_contains(const std::string &key) {
    auto el = files_.find(key);
    return el != files_.end();
}

Session::Session(int session_id) : session_id(session_id) {}

std::string Session::get_cookie() const {
    return Utils::sign_cookie(std::to_string(this->session_id)); // TODO: make this more secure
}

bool Session::is_logged_in() const {
    return this->login_status;
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

const std::vector<std::string> *Session::get_paths() {
    std::vector<std::string> paths;
    paths.reserve(files_.size());
    for(const auto& it : files_){
        paths.push_back(it.first);
        paths.emplace_back(it.first);
    }
    return new std::vector<std::string>(std::move(paths));
}

const std::unordered_map<std::string, std::string> *Session::get_files() {
    return new std::unordered_map<std::string, std::string>(files_);
}
