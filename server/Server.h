//
// Created by stevezbiz on 07/11/20.
//

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include "Session.h"

class Server {
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;

    void do_accept();

public:
    Server(boost::asio::io_context &io_service, const boost::asio::ip::tcp::endpoint &endpoint);
};

#endif //SERVER_SERVER_H
