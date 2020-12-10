//
// Created by stevezbiz on 05/12/20.
//

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
    if (sqlite3_prepare_v2(db_, "SELECT COUNT(*) FROM USERS WHERE Username = ? AND Password = ?",
                           -1, &query, nullptr) != SQLITE_OK)
        return false;
    if (sqlite3_bind_text(query, 1, username.data(), -1, nullptr) != SQLITE_OK) return false;
    if (sqlite3_bind_text(query, 2, password.data(), -1, nullptr) != SQLITE_OK) return false;
    // execute the query
    if (sqlite3_step(query) != SQLITE_OK) return false;
    // verify the result of the query
    int count = sqlite3_column_int(query, 0);
    if (count != 1) return false;
    if (sqlite3_finalize(query) != SQLITE_OK) return false;
    return true;
}
