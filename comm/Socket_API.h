//
// Created by Paolo Gastaldi on 21/11/2020.
//

#ifndef SERVER_SOCKET_API_H
#define SERVER_SOCKET_API_H

#include "Message.h"

class Socket_API {
    boost::asio::ip::tcp::socket socket_;

    /**
     * testing handler
     * @param ec
     * @param bytes_transferred
     */
    static void generic_handler(const boost::system::error_code& ec, std::size_t bytes_transferred) {
        std::cout << ec << " errors, " <<bytes_transferred << " bytes as been transferred correctly" << std::endl;
    }

public:
    /**
     * constructor
     * @param socket
     */
    explicit Socket_API(boost::asio::io_service& socket) : socket_(socket) {}

    /**
     * socket setter
     * @param socket
     */
    void set_socket(boost::asio::ip::tcp::socket &&socket) {
        if(this->socket_.is_open())
            this->socket_.close();
        this->socket_ = std::move(socket);
    }

    /**
     * socket getter
     * @return socket
     */
    boost::asio::ip::tcp::socket &&get_socket() {
        return std::move(this->socket_);
    }

    /**
     * send a Message
     * (sync mode by default)
     * wait until the message as been sent
     * @tparam Handler
     * @param message
     * @param handler
     */
    template <typename Handler>
    void send(Message *message = new Message{ ERROR }, Handler handler = generic_handler) {
        boost::asio::write(this->socket_, message->send(), handler);
    }

    /**
     * async send a Message
     * @tparam Handler
     * @param message
     * @param handler
     */
    template <typename Handler>
    void async_send(Message *message = new Message{ ERROR }, Handler handler = generic_handler) {
        boost::asio::async_write(this->socket_, message->send(), handler); // deferred or async ? The system chooses
    }

    /**
     * receive a message
     * @tparam Handler
     * @param expectedMessage
     * @param handler
     * @return message
     */
    template <typename Handler>
    Message *receive(Message *expectedMessage = new Message{ ERROR }, Handler handler = generic_handler) {
        auto message = new Message{};
        boost::asio::read(this->socket_, message->get_header_buffer(), handler);

        message->build(); // build the header
        if(message->code != expectedMessage->code)
            return new Message{ ERROR };

        boost::asio::read(socket_, message->get_content_buffer(), handler);

        return message->build(); // build the whole message
    }

    ~Socket_API() {
        if(this->socket_.is_open())
            this->socket_.close();
    }
};


#endif //SERVER_SOCKET_API_H
