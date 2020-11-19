//
// Created by stevezbiz on 07/11/20.
//

#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H


#include "Message.h"
#include <boost/asio.hpp>
#include <array>
#include <fstream>
#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <deque>

class Session : public std::enable_shared_from_this<Session> {
    boost::asio::ip::tcp::socket socket_;
    Message read_msg_;
    std::deque<Message> write_msgs_;

    enum { MaxLength = 40960 };
    std::array<char, MaxLength> m_buf;
    boost::asio::streambuf m_requestBuf_;
    std::ofstream m_outputFile;
    size_t m_fileSize;
    std::string m_fileName;

    void do_read_header();

    void do_read_body();

    void do_write();

    void do_read_file();

    void process_read(size_t t_bytesTransferred);

    void create_file();

    void do_read_file_header(std::istream &stream);

    void do_read_file_body(size_t t_bytesTransferred);

public:
    Session(boost::asio::ip::tcp::socket socket);

    void start();

    void write(const Message &msg);
};

#endif //SERVER_SESSION_H
