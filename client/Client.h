//
// Created by stevezbiz on 07/11/20.
//

#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include <boost/filesystem/path.hpp>
#include <boost/log/trivial.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <deque>
#include <array>
#include <fstream>
#include "Message.h"

class Client {
    boost::asio::io_context &ctx_;
    boost::asio::ip::tcp::socket socket_;
    Message read_msg_;
    std::deque<Message> write_msgs_;

    enum { MessageSize = 1024 };
    std::array<char, MessageSize> m_buf;
    boost::asio::streambuf m_request;
    std::ifstream m_sourceFile;
    std::string m_path;

    void do_connect(boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

    void do_read_header();

    void do_read_body();

    void do_write();

    void openFile(std::string const& t_path);

    void doWriteFile(const boost::system::error_code& t_ec);

    template<class Buffer>
    void writeBuffer(Buffer& t_buffer);

public:
    Client(boost::asio::io_context &ctx, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

    void write(const Message &msg);

    void close();
};

template<class Buffer>
void Client::writeBuffer(Buffer& t_buffer)
{
    //std::cout << "Write buffer" << std::endl;
    boost::asio::async_write(socket_,
                             t_buffer,
                             [this](boost::system::error_code ec, size_t /*length*/)
                             {
                                 doWriteFile(ec);
                             });
}


#endif //CLIENT_CLIENT_H
