//
// Created by stevezbiz on 07/11/20.
//

#include "Server.h"

Server::Server(boost::asio::io_context &ctx, const boost::asio::ip::tcp::endpoint &endpoint)
        : acceptor_(ctx, endpoint), socket_(ctx) {
    Server_API::set_login(login);
    Server_API::set_end(end);
    Server_API::set_get(get);
    Server_API::set_probe(probe);
    Server_API::set_push(push);
    Server_API::set_restore(restore);
    Server_API::set_end(end);
    Server_API::set_handle_error(handle_error);

    do_accept();
}

void Server::do_accept() {
    acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
        if (!ec) {
            Logger::info("Server::do_accept", "New connection accepted", PR_HIGH);
            std::make_shared<Session>(std::move(socket_))->start();
        }
        do_accept();
    });
}

bool Server::login(const std::string &, const std::string &) {
    return true;
}

const std::map<std::string, std::string> *Server::probe(const std::vector<std::string> &) {
    return new std::map<std::string, std::string>();
}

const std::vector<unsigned char> *Server::get(const std::string &) {
    return new std::vector<unsigned char>();
}

bool Server::push(const std::string &, const std::vector<unsigned char> &, const std::string &, ElementStatus) {
    return true;
}

const std::vector<std::string> *Server::restore() {
    return new std::vector<std::string>();
}

bool Server::end() {
    return true;
}

void Server::handle_error(const Comm_error *comm_error) {
    Logger::error(comm_error);
    // std::cout << comm_error->to_string() << std::endl;
}
