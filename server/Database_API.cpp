//
// Created by stevezbiz on 05/12/20.
//

#include <vector>
#include "Database_API.h"
#include "../comm/Logger.h"

Database_API::Database_API(std::string path) {
    int rc = sqlite3_open_v2(path.data(), &db_, SQLITE_OPEN_READWRITE, nullptr);
    if (rc != SQLITE_OK) {
        Logger::error("Database_API::Database_API", "sqlite3_open_v2: error " + std::to_string(rc), PR_VERY_HIGH);
        // TODO: error management
    }
}

Database_API::~Database_API() {
    int rc = sqlite3_close_v2(db_);
    if (rc != SQLITE_OK) {
        Logger::error("Database_API::~Database_API", "sqlite3_close_v2: error " + std::to_string(rc), PR_VERY_HIGH);
        // TODO: error management
    }
}

bool Database_API::login_query(const std::string &username, const std::string &password) const {
    // define the query
    int rc;
    sqlite3_stmt *query;
    rc = sqlite3_prepare_v2(db_, "SELECT COUNT(*) FROM USERS WHERE username = ? AND password = ?", -1, &query, nullptr);
    if (rc != SQLITE_OK) {
        Logger::error("Database_API::login_query", "sqlite3_prepare_v2: error " + std::to_string(rc), PR_HIGH);
        return false;
    }
    rc = sqlite3_bind_text(query, 1, username.data(), -1, nullptr);
    if (rc != SQLITE_OK) {
        Logger::error("Database_API::login_query", "sqlite3_bind_text: error " + std::to_string(rc), PR_HIGH);
        return false;
    }
    rc = sqlite3_bind_text(query, 2, password.data(), -1, nullptr);
    if (rc != SQLITE_OK) {
        Logger::error("Database_API::login_query", "sqlite3_bind_text: error " + std::to_string(rc), PR_HIGH);
        return false;
    }
    // execute the query
    rc = sqlite3_step(query);
    if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
        Logger::error("Database_API::login_query", "sqlite3_step: error " + std::to_string(rc), PR_HIGH);
        return false;
    }
    // verify the result of the query
    int count = sqlite3_column_int(query, 0);
    if (count != 1) {
        return false;
    }
    rc = sqlite3_finalize(query);
    if (rc != SQLITE_OK) {
        Logger::error("Database_API::login_query", "sqlite3_finalize: error " + std::to_string(rc), PR_HIGH);
        return false;
    }
    return true;
}

std::vector<std::string> *Database_API::get_path_schema(const std::string &username) const {
    int rc;
    sqlite3_stmt *query;
    // select
    std::vector<std::string> res;
    rc = sqlite3_prepare_v2(db_, "SELECT path FROM SESSIONS WHERE user = ?", -1, &query, nullptr);
    if (rc != SQLITE_OK) {
        Logger::error("Database_API::get_path_schema", "sqlite3_prepare_v2: error " + std::to_string(rc), PR_HIGH);
        return new std::vector<std::string>(res);
    }
    rc = sqlite3_bind_text(query, 1, username.data(), -1, nullptr);
    if (rc != SQLITE_OK) {
        Logger::error("Database_API::get_path_schema", "sqlite3_bind_text: error " + std::to_string(rc), PR_HIGH);
        return new std::vector<std::string>(res);
    }
    while (rc != SQLITE_DONE) {
        rc = sqlite3_step(query);
        if (rc == SQLITE_ROW) {
            res.emplace_back(reinterpret_cast<const char *>(sqlite3_column_text(query, 0)));
        } else if (rc != SQLITE_DONE) {
            Logger::error("Database_API::get_path_schema", "sqlite3_step: error " + std::to_string(rc), PR_HIGH);
            return new std::vector<std::string>(res);
        }
    }
    rc = sqlite3_finalize(query);
    if (rc != SQLITE_OK) {
        Logger::error("Database_API::get_path_schema", "sqlite3_finalize: error " + std::to_string(rc), PR_HIGH);
        return new std::vector<std::string>(res);
    }
    return new std::vector<std::string>(res);
}

bool Database_API::insert_path(const std::string &path, const std::string &hash, const std::string &username) const {
    int rc;
    sqlite3_stmt *query;
    rc = sqlite3_prepare_v2(db_, "INSERT INTO SESSIONS(path,hash,user) values(?,?,?)", -1, &query, nullptr);
    if (rc != SQLITE_OK) {
        Logger::error("Database_API::insert_path", "sqlite3_prepare_v2: error " + std::to_string(rc), PR_HIGH);
        return false;
    }
    rc = sqlite3_bind_text(query, 1, path.data(), -1, nullptr);
    if (rc != SQLITE_OK) {
        Logger::error("Database_API::insert_path", "sqlite3_bind_text: error " + std::to_string(rc), PR_HIGH);
        return false;
    }
    rc = sqlite3_bind_text(query, 2, hash.data(), -1, nullptr);
    if (rc != SQLITE_OK) {
        Logger::error("Database_API::insert_path", "sqlite3_bind_text: error " + std::to_string(rc), PR_HIGH);
        return false;
    }
    rc = sqlite3_bind_text(query, 3, username.data(), -1, nullptr);
    if (rc != SQLITE_OK) {
        Logger::error("Database_API::insert_path", "sqlite3_bind_text: error " + std::to_string(rc), PR_HIGH);
        return false;
    }
    rc = sqlite3_step(query);
    if (rc != SQLITE_DONE) {
        Logger::error("Database_API::insert_path", "sqlite3_step: error " + std::to_string(rc), PR_HIGH);
        return false;
    }
    rc = sqlite3_finalize(query);
    if (rc != SQLITE_OK) {
        Logger::error("Database_API::insert_path", "sqlite3_finalize: error " + std::to_string(rc), PR_HIGH);
        return false;
    }
    return true;
}

bool Database_API::delete_path(const std::string &path) const {
    int rc;
    sqlite3_stmt *query;
    rc = sqlite3_prepare_v2(db_, "DELETE FROM SESSIONS WHERE path = ?", -1, &query, nullptr);
    if (rc != SQLITE_OK) {
        Logger::error("Database_API::delete_path", "sqlite3_prepare_v2: error " + std::to_string(rc), PR_HIGH);
        return false;
    }
    rc = sqlite3_bind_text(query, 1, path.data(), -1, nullptr);
    if (rc != SQLITE_OK) {
        Logger::error("Database_API::delete_path", "sqlite3_bind_text: error " + std::to_string(rc), PR_HIGH);
        return false;
    }
    rc = sqlite3_step(query);
    if (rc != SQLITE_DONE) {
        Logger::error("Database_API::delete_path", "sqlite3_step: error " + std::to_string(rc), PR_HIGH);
        return false;
    }
    rc = sqlite3_reset(query);
    if (rc != SQLITE_OK) {
        Logger::error("Database_API::delete_path", "sqlite3_finalize: error " + std::to_string(rc), PR_HIGH);
        return false;
    }
    return true;
}

bool Database_API::update_path(const std::string &path, const std::string &hash) const {
    int rc;
    sqlite3_stmt *query;
    rc = sqlite3_prepare_v2(db_, "UPDATE SESSIONS SET hash = ? WHERE path = ?", -1, &query, nullptr);
    if (rc != SQLITE_OK) {
        Logger::error("Database_API::update_path", "sqlite3_prepare_v2: error " + std::to_string(rc), PR_HIGH);
        return false;
    }
    rc = sqlite3_bind_text(query, 1, hash.data(), -1, nullptr);
    if (rc != SQLITE_OK) {
        Logger::error("Database_API::update_path", "sqlite3_bind_text: error " + std::to_string(rc), PR_HIGH);
        return false;
    }
    rc = sqlite3_bind_text(query, 2, path.data(), -1, nullptr);
    if (rc != SQLITE_OK) {
        Logger::error("Database_API::update_path", "sqlite3_bind_text: error " + std::to_string(rc), PR_HIGH);
        return false;
    }
    rc = sqlite3_step(query);
    if (rc != SQLITE_DONE) {
        Logger::error("Database_API::update_path", "sqlite3_step: error " + std::to_string(rc), PR_HIGH);
        return false;
    }
    rc = sqlite3_finalize(query);
    if (rc != SQLITE_OK) {
        Logger::error("Database_API::update_path", "sqlite3_finalize: error " + std::to_string(rc), PR_HIGH);
        return false;
    }
    return true;
}