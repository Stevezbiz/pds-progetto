//
// Created by Paolo Gastaldi on 21/11/2020.
//

#ifndef SERVER_SOCKET_API_H
#define SERVER_SOCKET_API_H

#include "Message.h"
#include "Comm_error.h"

/**
 * list of ways to manage error in communication
 */
enum ERROR_MANAGEMENT : int {
    NO_RETRY = 0,
    RETRY_ONCE = 1,
    RETRY_TWICE = 2
};

/**
 * wrapper class over boost::asio read/write on sockets
 * to manage Message type message and standard Comm_error type errors
 */
class Socket_API {
    boost::asio::ip::tcp::socket socket_;
    Message *last_message;
    Comm_error last_error;
    int n_retry;
    int retry_delay;

    /**
     * call read/write functions and manage errors
     * @tparam R
     * @tparam Handler
     * @param perform_this
     * @param buffer
     * @return status
     */
    template <typename R, typename Handler>
    bool call_(std::function<R(boost::asio::io_service, boost::asio::buffer, Handler)> perform_this, boost::asio::buffer buffer) {
        bool status = true;
        int retry_cont = 0;

        auto err_cb = [ this->n_retry, this->retry_delay, &status, &retry_cont, &this->last_error ](const boost::system::error_code &ec) {
            if (ec) {
                if () {
                    std::this_thread::sleep_for(std::chrono::milliseconds(retry_delay));
                    perform_this(this->socket_, buffer, err_cb);
                }
            };
        };

        perform_this(this->socket_, buffer, err_cb);

        return status;
    }

    /**
     * testing handler
     * @param ec
     * @param bytes_transferred
     */
    static void generic_handler(const boost::system::error_code& ec, std::size_t bytes_transferred) {
        std::cout << ec << " errors, " <<bytes_transferred << " bytes as been transferred correctly" << std::endl;
    }

    /**
     * receive the packet header
     * @tparam Handler
     * @param expectedMessage
     * @param handler
     * @return status
     */
    template <typename Handler>
    bool receive_header_(MESSAGE_TYPE expectedMessage = UNDEFINED, Handler handler) {
        this->message = new Message{};

        if!(this->call_(boost::asio::read, this->socket_, this->message->get_header_buffer(), handler);
            return false;

        this->message->build(); // build the header
        if(expectedMessage != UNDEFINED && this->message->code != expectedMessage) {
            this->last_error = new Error{ UNEXPECTED_TYPE, "Socket_API::receive_header_", "Expected message code : " + expectedMessage};
            this->last_message = new Message{ this->last_error };
            return false;
        }
        return true;
    }

    /**
     * receive the packet content
     * @tparam Handler
     * @param handler
     * @return status
     */
    template <typename Handler>
    bool receive_content_(Handler handler) {
        if!(this->call_(boost::asio::read, socket_, this->message->get_content_buffer(), handler))
            return false;

        this-> message = message->build(); // build the whole message

        return true;
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
    Message *receive(MESSAGE_TYPE expectedMessage = UNDEFINED, Handler handler = generic_handler) {
        auto message = new Message{};
        boost::asio::read(this->socket_, message->get_header_buffer(), handler);

        message->build(); // build the header
        if(expectedMessage != UNDEFINED && message->code != expectedMessage)
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
