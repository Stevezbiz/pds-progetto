//
// Created by stevezbiz on 07/11/20.
//

#include <iostream>
#include "Session.h"

Session::Session(boost::asio::ip::tcp::socket socket) : socket_(std::move(socket)) {}

void Session::start() {
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
    boost::asio::async_read(socket_,
                            boost::asio::buffer(read_msg_.getData(), Message::header_length),
                            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                                if (!ec && read_msg_.decodeHeader()) {
                                    do_read_body();
                                } else {

                                }
                            });
}

void Session::do_read_body() {
    auto self(shared_from_this());
    boost::asio::async_read(socket_,
                            boost::asio::buffer(read_msg_.getBody(), read_msg_.getBodyLength()),
                            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
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
    boost::asio::async_write(socket_,
                             boost::asio::buffer(write_msgs_.front().getData(),
                                                 write_msgs_.front().getLength()),
                             [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                                 if (!ec) {
                                     write_msgs_.pop_front();
                                     if (!write_msgs_.empty()) {
                                         do_write();
                                     }
                                 } else {

                                 }
                             });
}
