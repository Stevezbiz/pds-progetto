//
// Created by stevezbiz on 26/11/20.
//

#include "Socket_API.h"

#include <utility>

bool Socket_API::call_(const std::function<void(boost::asio::ip::tcp::socket &, boost::system::error_code &)> &perform_this) {
    bool status = false;
    bool stop = false;
    int retry_cont = 0;
    boost::system::error_code ec;

    try {
        while (!stop && retry_cont <= this->n_retry_) {
            if(retry_cont > 0)
                Logger::warning("Socket_API::call_", "Retry " + std::to_string(retry_cont), PR_NORMAL);
            perform_this(*this->socket_, ec);

            if(!ec.failed()) {
                stop = true;
                status = true;
                ec.clear();
            } else {
                this->comm_error = new Comm_error{ CE_FAILURE, "Socket_API::call_", "Operation failure on socket", ec };
                std::this_thread::sleep_for(std::chrono::milliseconds(retry_delay_));
                retry_cont++;
            }
        }
    } catch(const std::exception &ec) {
        this->comm_error = new Comm_error{ CE_GENERIC, "Socket_API::call_", "Exception caught: " + std::string{ ec.what() }}; // never used
        return false;
    }

    return status;
}

bool Socket_API::generic_handler_(const boost::system::error_code &ec, std::size_t bytes_transferred) {
    std::cout << ec << " errors, " <<bytes_transferred << " bytes as been transferred correctly" << std::endl;
    return ec ? true : false;
}

bool Socket_API::receive_header_() {
    if(!this->call_([this](boost::asio::ip::tcp::socket &socket, boost::system::error_code &ec) {
            boost::asio::read(*this->socket_, this->message->get_header_buffer(), ec);
        })) {
        this->comm_error = new Comm_error{CE_FAILURE, "Socket_API::receive_header_", "Unable to read message header" };
        return false;
    }

    return true;
}

bool Socket_API::receive_content_() {
    if(!this->call_([this](boost::asio::ip::tcp::socket &socket, boost::system::error_code &ec) {
            boost::asio::read(*this->socket_, this->message->get_content_buffer(), ec);
        })) {
        this->comm_error = new Comm_error{CE_FAILURE, "Socket_API::receive_content_", "Unable to read message content" };
        return false;
    }

    return true;
}

Socket_API::Socket_API(std::string ip, std::string port, ERROR_MANAGEMENT error_management, long retry_delay, bool keep_alive) :
        ip(std::move(ip)),
        port(std::move(port)),
        n_retry_(error_management),
        retry_delay_(retry_delay),
        keep_alive_(keep_alive) {}

Socket_API::Socket_API(boost::asio::ip::tcp::socket socket, ERROR_MANAGEMENT error_management, long retry_delay, bool keep_alive) :
        n_retry_(error_management),
        retry_delay_(retry_delay),
        keep_alive_(keep_alive) {
    this->socket_ = new boost::asio::ip::tcp::socket{ std::move(socket) };
}

bool Socket_API::open_conn(bool force) {
    Logger::info("Socket_API::open_conn", "Opening a connection...", PR_VERY_LOW);

    if(this->socket_ != nullptr && this->socket_->is_open() && this->keep_alive_ && !force) {
        Logger::info("Socket_API::open_conn", "Connection already opened", PR_VERY_LOW);
        return true;
    }

    if(!this->close_conn()) // close current socket in a safe way
        return false;

    try {
        Logger::info("Socket_API::open_conn", "Creating the socket...", PR_VERY_LOW);
        boost::asio::io_context ctx;
        boost::asio::ip::tcp::resolver resolver(ctx);
        auto endpoint_iterator = resolver.resolve({ this->ip, this->port });
        boost::asio::ip::tcp::socket socket{ ctx };
        boost::asio::connect(socket, endpoint_iterator);
        this->socket_ = new boost::asio::ip::tcp::socket{ std::move(socket) };
    } catch(const std::exception &e) {
        this->comm_error = new Comm_error{ CE_FAILURE, "Socket_API::open_conn", e.what() };
        return false;
    }
    Logger::info("Socket_API::open_conn", "Opening a connection... - done", PR_VERY_LOW);

    return true;
}

bool Socket_API::send(Message *message) {
    Logger::info("Socket_API::send", "Sending a message...", PR_LOW);
//    delete this->message;
//    delete this->comm_error;

    message->keep_alive = this->keep_alive_;
    if(!this->call_([this, &message](boost::asio::ip::tcp::socket &socket, boost::system::error_code &ec) {
            boost::asio::write(*this->socket_, message->send(), ec);
        })) {
        this->comm_error = new Comm_error{CE_FAILURE, "Socket_API::send", "Unable to write message" };
        return false;
    }
    Logger::info("Socket_API::send", "Sending a message... - done", PR_LOW);
//    delete this->message;

    return true;
}

bool Socket_API::receive(MESSAGE_TYPE expectedMessage) {
//    delete this->message;
//    delete this->comm_error;

    bool status = true;
    this->message = new Message{};
    Logger::info("Socket_API::receive", "Receiving a message...", PR_LOW);

    if(!this->receive_header_())
        return false; // it cannot do any other action here

    try {
        this->message = this->message->build_header(); // build the header
    } catch(const std::exception &ec) {
        this->comm_error = new Comm_error{ CE_FAILURE, "Socket_API::receive", "Cannot build the message header: " + std::string{ ec.what() }};
        return false; // it cannot do any other action here
    }

    if(expectedMessage != MSG_UNDEFINED && message->code != expectedMessage && message->code != MSG_ERROR) {
        this->comm_error = new Comm_error{CE_UNEXPECTED_TYPE, "Socket_API::receive_header_", "Expected message code " + std::to_string(expectedMessage) + " but actual code is " + std::to_string(message->code) };
        this->message = Message::error(this->comm_error);
        status = false;
    }

    if(!this->receive_content_()) // read the message content in any case
        return false; // it cannot do any other action here

    try {
        auto new_message = message->build_content(); // build the whole message
//        delete this->message;
        this->message = new_message;
    } catch(const std::exception &ec) {
        this->comm_error = new Comm_error{ CE_FAILURE, "Socket_API::receive", "Cannot build the message content: " + std::string{ ec.what() }};
        return false; // it cannot do any other action here
    }
    Logger::info("Socket_API::receive", "Receiving a message... - done", PR_LOW);

    return status;
}

bool Socket_API::close_conn(bool force) {
    Logger::info("Socket_API::close_conn", "Closing a connection...", PR_VERY_LOW);

    if(this->socket_ == nullptr)
        return true;
    if(this->socket_->is_open() && this->keep_alive_ && !force)
        return true;
    Logger::info("Socket_API::close_conn", "Force closing", PR_VERY_LOW);

    try {
        if(this->socket_->is_open()) {
            this->socket_->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
            this->socket_->close();
        }
    } catch(const std::exception &e) {
        this->comm_error = new Comm_error{ CE_FAILURE, "Socket_API::close_conn", e.what() };
        return false;
    }

    delete this->socket_;
    this->socket_ = nullptr;
    Logger::info("Socket_API::close_conn", "Closing a connection... - done", PR_VERY_LOW);
    return true;
}

Message *Socket_API::get_message() const {
    return this->message;
}
Comm_error *Socket_API::get_last_error() const {
    return this->comm_error ? this->comm_error : this->message->comm_error;
}

Socket_API::~Socket_API() {
    if(this->socket_->is_open()) {
        this->socket_->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        this->socket_->close();
    }
    delete this->socket_;
    delete this->message;
    delete this->comm_error;
}

