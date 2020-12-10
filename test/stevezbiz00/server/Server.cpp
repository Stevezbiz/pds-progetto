//
// Created by stevezbiz on 07/11/20.
//

#include "Server.h"

Server::Server(boost::asio::io_context &io_service, const boost::asio::ip::tcp::endpoint &endpoint)
        : acceptor_(io_service, endpoint),
          socket_(io_service) {
    do_accept();
}

void Server::do_accept() {
    acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
        if (!ec) {
            std::make_shared<Session>(std::move(socket_))->start();
        }
        do_accept();
    });
}