//
// Created by stevezbiz on 07/11/20.
//

#include "Client.h"

#include <utility>

Client::Client(boost::asio::io_service &io_service, boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
        : io_service_(io_service),
          socket_(io_service) {
    do_connect(std::move(endpoint_iterator));
}

void Client::write(const Message &msg) {
    io_service_.post([this, msg]() {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress) {
            do_write();
        }
    });
}

void Client::close() {
    io_service_.post([this]() { socket_.close(); });
}

void Client::do_connect(boost::asio::ip::tcp::resolver::iterator endpoint_iterator) {
    boost::asio::async_connect(socket_, std::move(endpoint_iterator),
                               [this](boost::system::error_code ec,
                                      const boost::asio::ip::tcp::resolver::iterator &it) {
                                   if (!ec) {
                                       do_read_header();
                                   }
                               });
}

void Client::do_read_header() {
    boost::asio::async_read(socket_,
                            boost::asio::buffer(read_msg_.getData(), Message::header_length),
                            [this](boost::system::error_code ec, std::size_t length) {
                                if (!ec && read_msg_.decodeHeader()) {
                                    do_read_body();
                                } else {
                                    socket_.close();
                                }
                            });
}

void Client::do_read_body() {
    boost::asio::async_read(socket_,
                            boost::asio::buffer(read_msg_.getBody(), read_msg_.getBodyLength()),
                            [this](boost::system::error_code ec, std::size_t length) {
                                if (!ec) {
                                    std::cout.write(read_msg_.getBody(), read_msg_.getBodyLength());
                                    std::cout << "\n";
                                    do_read_header();
                                } else {
                                    socket_.close();
                                }
                            });
}

void Client::do_write() {
    boost::asio::async_write(socket_,
                             boost::asio::buffer(write_msgs_.front().getData(), write_msgs_.front().getLength()),
                             [this](boost::system::error_code ec, std::size_t length) {
                                 if (!ec) {
                                     write_msgs_.pop_front();
                                     if (!write_msgs_.empty()) {
                                         do_write();
                                     }
                                 } else {
                                     socket_.close();
                                 }
                             });
}