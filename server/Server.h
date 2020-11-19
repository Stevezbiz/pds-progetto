//
// Created by stevezbiz on 07/11/20.
//

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include "Session.h"
#include <array>
#include <fstream>
#include <string>
#include <memory>
#include <boost/asio.hpp>

class Server {
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;

    std::string m_workDirectory;


    void do_accept();

    void create_work_dir();



public:
    Server(boost::asio::io_context &io_service, const boost::asio::ip::tcp::endpoint &endpoint);

};

#endif //SERVER_SERVER_H
