//
// Created by stevezbiz on 05/12/20.
//

#include <vector>
#include "Database_API.h"

Database_API::Database_API(std::string path) {
    if (sqlite3_open_v2(path.data(), &db_, SQLITE_OPEN_READWRITE, nullptr) != SQLITE_OK) {
        // TODO: error management
    }
}

Database_API::~Database_API() {
    if (sqlite3_close_v2(db_) != SQLITE_OK) {
        // TODO: error management
    }
}

bool Database_API::login_query(const std::string &username, const std::string &password) const {
    // define the query
    sqlite3_stmt *query;
    if (sqlite3_prepare_v2(db_, "SELECT COUNT(*) FROM USERS WHERE username = ? AND password = ?",
                           -1, &query, nullptr) != SQLITE_OK)
        return false;
    if (sqlite3_bind_text(query, 1, username.data(), -1, nullptr) != SQLITE_OK) return false;
    if (sqlite3_bind_text(query, 2, password.data(), -1, nullptr) != SQLITE_OK) return false;
    // execute the query
    int rc = sqlite3_step(query);
    if (rc != SQLITE_DONE && rc != SQLITE_ROW) return false;
    // verify the result of the query
    int count = sqlite3_column_int(query, 0);
    if (count != 1) return false;
    if (sqlite3_finalize(query) != SQLITE_OK) return false;
    return true;
}

std::vector<std::string> *Database_API::get_path_schema(const std::string &username) const {
    sqlite3_stmt *query;
    // select
    std::vector<std::string> res;
    if (sqlite3_prepare_v2(db_, "SELECT path FROM SESSIONS WHERE user = ?", -1, &query, nullptr)
        != SQLITE_OK) {
        // TODO: error management
    }
    if (sqlite3_bind_text(query, 1, username.data(), -1, nullptr) != SQLITE_OK) {
        // TODO: error management
    }
    while (sqlite3_step(query) != SQLITE_DONE) {
        res.emplace_back(reinterpret_cast<const char *>(sqlite3_column_text(query, 0)));
    }
    if (sqlite3_finalize(query) != SQLITE_OK) {
        // TODO: error management
    }
    return new std::vector<std::string>(res);
}

bool Database_API::save_path_schema(const std::unordered_map<std::string, std::string> &map,
                                    const std::string &username) const {
    sqlite3_stmt *query;
    // delete
    if (sqlite3_prepare_v2(db_, "DELETE FROM SESSIONS WHERE user = ?", -1, &query, nullptr) !=
        SQLITE_OK)
        return false;
    if (sqlite3_bind_text(query, 1, username.data(), -1, nullptr) != SQLITE_OK) return false;
    if (sqlite3_step(query) != SQLITE_DONE) return false;
    if (sqlite3_reset(query) != SQLITE_OK) return false;
    // insert
    bool first = true;
    if (sqlite3_prepare_v2(db_, "INSERT INTO SESSIONS(path,hash,user) values(?,?,?)", -1, &query, nullptr) !=
        SQLITE_OK)
        return false;
    if (sqlite3_bind_text(query, 3, username.data(), -1, nullptr) != SQLITE_OK) return false;
    for (auto el : map) {
        if (first && sqlite3_bind_text(query, 1, el.first.data(), -1, nullptr) != SQLITE_OK) return false;
        if (first && sqlite3_bind_text(query, 2, el.second.data(), -1, nullptr) != SQLITE_OK) return false;
        if (sqlite3_step(query) != SQLITE_DONE) return false;
        first = false;
    }
    if (sqlite3_finalize(query) != SQLITE_OK) return false;
    return true;
}

bool Database_API::insert_path(const std::string &path, const std::string &hash, const std::string &username) const {
    sqlite3_stmt *query;
    if (sqlite3_prepare_v2(db_, "INSERT INTO SESSIONS(path,hash,user) values(?,?,?)", -1, &query, nullptr) !=
        SQLITE_OK)
        return false;
    if (sqlite3_bind_text(query, 1, path.data(), -1, nullptr) != SQLITE_OK) return false;
    if (sqlite3_bind_text(query, 2, hash.data(), -1, nullptr) != SQLITE_OK) return false;
    if (sqlite3_bind_text(query, 3, username.data(), -1, nullptr) != SQLITE_OK) return false;
    if (sqlite3_step(query) != SQLITE_DONE) return false;
    if (sqlite3_finalize(query) != SQLITE_OK) return false;
    return true;
}

bool Database_API::delete_path(const std::string &path) const {
    sqlite3_stmt *query;
    if (sqlite3_prepare_v2(db_, "DELETE FROM SESSIONS WHERE path = ?", -1, &query, nullptr) !=
        SQLITE_OK)
        return false;
    if (sqlite3_bind_text(query, 1, path.data(), -1, nullptr) != SQLITE_OK) return false;
    if (sqlite3_step(query) != SQLITE_DONE) return false;
    if (sqlite3_reset(query) != SQLITE_OK) return false;
    return true;
}

bool Database_API::update_path(const std::string &path, const std::string &hash) const {
    sqlite3_stmt *query;
    if (sqlite3_prepare_v2(db_, "UPDATE SESSIONS SET hash = ? WHERE path = ?", -1, &query, nullptr) !=
        SQLITE_OK)
        return false;
    if (sqlite3_bind_text(query, 1, hash.data(), -1, nullptr) != SQLITE_OK) return false;
    if (sqlite3_bind_text(query, 2, path.data(), -1, nullptr) != SQLITE_OK) return false;
    if (sqlite3_step(query) != SQLITE_DONE) return false;
    if (sqlite3_finalize(query) != SQLITE_OK) return false;
    return true;
}