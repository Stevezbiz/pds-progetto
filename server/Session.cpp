//
// Created by stevezbiz on 07/11/20.
//

#include <iostream>
#include <sqlite3.h>
#include "Session.h"

using namespace boost::asio;

Session::Session(ip::tcp::socket socket) : socket_(std::move(socket)) {}

void Session::start() {
    authenticate_client();
    do_read_header();
}

void Session::write(const Message &msg) {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress) {
        do_write();
    }
}

void Session::do_read_header() {
    auto self(shared_from_this());
    async_read(socket_, buffer(read_msg_.getData(), Message::header_length),
               [this, self](boost::system::error_code ec, std::size_t length) {
                   if (!ec && read_msg_.decodeHeader()) {
                       do_read_body();
                   } else {

                   }
               });
}

void Session::do_read_body() {
    auto self(shared_from_this());
    async_read(socket_, buffer(read_msg_.getBody(), read_msg_.getBodyLength()),
               [this, self](boost::system::error_code ec, std::size_t length) {
                   if (!ec) {
                       std::cout << read_msg_.getBody() << std::endl;
                       //write(read_msg_);
                       do_read_header();
                   } else {

                   }
               });
}

void Session::do_write() {
    auto self(shared_from_this());
    async_write(socket_, buffer(write_msgs_.front().getData(), write_msgs_.front().getLength()),
                [this, self](boost::system::error_code ec, std::size_t length) {
                    if (!ec) {
                        write_msgs_.pop_front();
                        if (!write_msgs_.empty()) {
                            do_write();
                        }
                    } else {

                    }
                });
}

void Session::authenticate_client() {
    std::string username;
    std::string password;
    // TODO: ricevo le credenziali dal client
    if(do_authenticate(username, password)){
        // TODO: invio l'esito positivo al client
    } else {
        // TODO: invio l'esito negativo al client
        socket_.close();
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
    if(sqlite3_close_v2(db) != SQLITE_OK){
        return false;
    }
    user_ = username;
    return true;
}
