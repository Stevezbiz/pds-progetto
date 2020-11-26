//
// Created by stevezbiz on 07/11/20.
//

#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H

#include "../comm/Message.h"
#include <boost/asio.hpp>
#include <deque>

class Session : public std::enable_shared_from_this<Session> {
    boost::asio::ip::tcp::socket socket_;
    Message read_msg_;
    std::deque<Message> write_msgs_;
    std::string user_;

    void authenticate_client();

    void do_read_header();

    void do_read_body();

    void do_write();

    bool do_authenticate(std::string username, const std::string& password);

public:
    Session(boost::asio::ip::tcp::socket socket);

    void start();

    void write(const Message &msg);
};

#endif //SERVER_SESSION_H
