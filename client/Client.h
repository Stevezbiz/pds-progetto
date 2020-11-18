//
// Created by stevezbiz on 07/11/20.
//

#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include <boost/asio.hpp>
#include <iostream>
#include <deque>
#include "Message.h"

class Client {
    boost::asio::io_context &ctx_;
    boost::asio::ip::tcp::socket socket_;
    Message read_msg_;
    std::deque<Message> write_msgs_;

    void do_connect(boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

    void do_read_header();

    void do_read_body();

    void do_write();

public:
    Client(boost::asio::io_context &ctx, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

    void write(const Message &msg);

    void close();
};


#endif //CLIENT_CLIENT_H
