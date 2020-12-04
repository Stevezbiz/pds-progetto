//
// Created by stevezbiz on 07/11/20.
//

#include "Server.h"

#include <utility>

Server::Server(boost::asio::io_context &ctx, const boost::asio::ip::tcp::endpoint &endpoint, std::string db) : acceptor_(ctx, endpoint),
                                                                                               socket_(ctx), api_(std::move(db)),
                                                                                               stop_(false) {
    server_init();
    accept();
}

void Server::accept() {
    while (!stop_) {
        acceptor_.accept(socket_);
        std::thread thread([](Server_API &api, Session session) {
            api.run(session);
        }, std::ref(api_), std::move(socket_));
        thread.detach();
    }
}

bool Server::login(Session &session, const std::string &username, const std::string &password) {
    sqlite3 *db;
    // open the database
    if (sqlite3_open_v2("???", &db, SQLITE_OPEN_READONLY, nullptr) != SQLITE_OK) return false;
    // define the query
    sqlite3_stmt *query;
    if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM USERS WHERE Username = ? AND Password = ?",
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
    if (sqlite3_close_v2(db) != SQLITE_OK) return false;
    session.set_user(username);
    return true;
}

const std::map<std::string, std::string> *Server::probe(Session &, const std::vector<std::string> &) {
    return new std::map<std::string, std::string>();
}

const std::vector<unsigned char> *Server::get(Session &session, const std::string &path) {
    return new std::vector<unsigned char>();
}

bool Server::push(Session &session, const std::string &path, const std::vector<unsigned char> &file,
                  const std::string &hash, ElementStatus status) {
    return true;
}

const std::vector<std::string> *Server::restore(Session &session) {
    return new std::vector<std::string>();
}

bool Server::end(Session &session) {
    return true;
}

void Server::handle_error(Session &session, const Comm_error *comm_error) {
    // Logger::error(comm_error);
    Logger::error("Server::handle_error", comm_error->to_string(), PR_HIGH);
    // std::cout << comm_error->to_string() << std::endl;
}

void Server::server_init() {
    api_.set_login(login);
    api_.set_end(end);
    api_.set_get(get);
    api_.set_probe(probe);
    api_.set_push(push);
    api_.set_restore(restore);
    api_.set_end(end);
    api_.set_handle_error(handle_error);
}
