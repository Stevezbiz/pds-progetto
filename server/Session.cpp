//
// Created by stevezbiz on 07/11/20.
//

#include "Session.h"

using namespace boost::asio;

Session::Session(ip::tcp::socket socket) : api_(new Server_API{new Socket_API{std::move(socket)}}) {}

void Session::start() {
    authenticate_client();
    api_->run();
}

void Session::authenticate_client() {
    std::string username;
    std::string password;
    // TODO: ricevo le credenziali dal client
    if (do_authenticate(username, password)) {
        // TODO: invio l'esito positivo al client
    } else {
        // TODO: invio l'esito negativo al client
    }
}

bool Session::do_authenticate(std::string username, const std::string &password) {
    sqlite3 *db;
    // apro il database
    if (sqlite3_open_v2("tutorial.db", &db, SQLITE_OPEN_READONLY, nullptr) != SQLITE_OK) {
        return false;
    }
    // definisco la query al database
    sqlite3_stmt *query;
    if (sqlite3_prepare_v2(db, "SELECT Password FROM USERS WHERE Username = ?", -1, &query, nullptr) != SQLITE_OK) {
        return false;
    }
    if (sqlite3_bind_text(query, 1, username.data(), -1, nullptr) != SQLITE_OK) {
        return false;
    }
    // eseguo la query
    if (sqlite3_step(query) != SQLITE_OK) {
        return false;
    }
    int n = sqlite3_column_count(query);
    std::string res;
    // verifico l'esito della query
    switch (n) {
        case 0:
            return false;
        case 1:
            res = reinterpret_cast<const char *>(sqlite3_column_text(query, 0));
            if (res != password) {
                return false;
            }
            break;
        default:
            return false;
    }
    if (sqlite3_finalize(query) != SQLITE_OK) {
        return false;
    }
    if (sqlite3_close_v2(db) != SQLITE_OK) {
        return false;
    }
    user_ = username;
    return true;
}
