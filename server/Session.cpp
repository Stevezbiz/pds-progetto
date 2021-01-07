//
// Created by stevezbiz on 07/11/20.
//

#include "Session.h"
#include "../comm/Utils.h"

Session::Session(int session_id) : session_id(session_id) {}

std::string Session::get_cookie() const {
    return Utils::sign_cookie(std::to_string(session_id));
}

bool Session::is_logged_in() const {
    return login_status;
}

bool Session::create_file(const Database_API &database, const std::string &path, const std::string &hash) const {
    return database.insert_path(path, hash, user,PathType::file);
}

bool Session::modify_file(const Database_API &database, const std::string &path, const std::string &hash) {
    return database.update_path(path, hash);
}

bool Session::remove_file(const Database_API &database, const std::string &path) {
    return database.delete_path(path);
}

const std::vector<std::string> *Session::get_path_schema(const Database_API &database) const {
    return database.get_path_schema(user);
}

bool Session::create_dir(const Database_API &database, const std::string &path, const std::string &hash) const {
    return database.insert_path(path, hash, user, PathType::dir);
}

bool Session::remove_dir(const Database_API &database, const std::string &path) {
    return database.delete_path(path);
}

const std::unordered_map <std::string, std::string> *Session::get_schema(const Database_API &database) const {
    return database.get_schema(user);
}
