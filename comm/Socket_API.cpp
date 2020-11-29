//
// Created by stevezbiz on 26/11/20.
//

#include "Socket_API.h"

bool Socket_API::call_(const std::function<void(boost::asio::ip::tcp::socket &, boost::system::error_code &)> &perform_this) {
    bool status = false;
    bool stop = false;
    int retry_cont = 0;
    boost::system::error_code ec;

    while(!stop && retry_cont < this->n_retry) {
        perform_this(this->socket_, ec);

        if(!ec) {
            stop = true;
            status = true;
        }
        else
            std::this_thread::sleep_for(std::chrono::milliseconds(retry_delay));

        retry_cont++;
    }

    if(!status)
        this->comm_error = new Comm_error{FAILURE, "Socket_API", "Connection failure" };

    return status;
}

bool Socket_API::generic_handler_(const boost::system::error_code &ec, std::size_t bytes_transferred) {
    std::cout << ec << " errors, " <<bytes_transferred << " bytes as been transferred correctly" << std::endl;
    return ec ? true : false;
}

bool Socket_API::receive_header_() {
    this->message = new Message{};

    if(!this->call_([this](boost::asio::ip::tcp::socket &socket, boost::system::error_code &ec) {
            boost::asio::read(this->socket_, this->message->get_header_buffer(), ec);
        }))
        return false;

    return true;
}
bool Socket_API::receive_content_() {
    char s[10];
    boost::system::error_code ec;
    boost::asio::read(this->socket_, boost::asio::mutable_buffer(s, 10), ec);
    if(!this->call_([this](boost::asio::ip::tcp::socket &socket, boost::system::error_code &ec) {
            boost::asio::read(socket, this->message->get_content_buffer(), ec);
        }))
        return false;

    return true;
}

Socket_API::Socket_API(boost::asio::ip::tcp::socket &&socket, ERROR_MANAGEMENT error_management, long retry_delay) :
        socket_(std::move(socket)),
        n_retry(error_management),
        retry_delay(retry_delay),
        message(nullptr),
        comm_error(nullptr) {}

void Socket_API::set_socket(boost::asio::ip::tcp::socket &&socket) {
    if(this->socket_.is_open())
        this->socket_.close();
    this->socket_ = std::move(socket);
}

boost::asio::ip::tcp::socket &&Socket_API::get_socket() {
    return std::move(this->socket_);
}

bool Socket_API::send(Message *message) {
    this->call_([&message](boost::asio::ip::tcp::socket &socket, boost::system::error_code &ec) {
        boost::asio::write(socket, message->send(), ec);
    });

    return true;
}

bool Socket_API::receive(MESSAGE_TYPE expectedMessage) {
    bool status = true;
    this->message = new Message{};

    if(!this->receive_header_())
        return false;

    this->message = this->message->build_header(); // build the header

    if(this->message->code == ERROR) {
        status = false;
    }
    else if(expectedMessage != UNDEFINED && message->code != expectedMessage) {
        this->comm_error = new Comm_error{ UNEXPECTED_TYPE, "Socket_API::receive_header_", "Expected message code : " + std::to_string(expectedMessage) };
        this->message = Message::error(this->comm_error);
        status = false;
    }

    if(!this->receive_content_()) // read the message content in any case
        status = false;

    this->message = message->build_content(); // build the whole message

    return status;
}

Message *Socket_API::get_message() {
    return this->message;
}
Comm_error *Socket_API::get_last_error() {
    return this->comm_error;
}

Socket_API::~Socket_API() {
    if(this->socket_.is_open())
        this->socket_.close();
    delete this->message;
    delete this->comm_error;
}

