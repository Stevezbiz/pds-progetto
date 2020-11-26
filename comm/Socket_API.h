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

        auto err_cb = [ this->socket_, buffer, this->n_retry, this->retry_delay, &status, &retry_cont, &this->last_error ](const boost::system::error_code &ec) {
            if(ec) {
                if(retry_cont < n_retry) {
                    retry_cont++;

                    std::this_thread::sleep_for(std::chrono::milliseconds(retry_delay));
                    perform_this(socket_, buffer, err_cb);
                }
                else {
                    status = false;
                    last_error = new Comm_error{ FAILURE, "Socket_API", "Connection failure" };
                }
            };
        };

        perform_this(this->socket_, buffer, err_cb);
        return status;
    }

    static void generic_handler(const boost::system::error_code &ec, std::size_t bytes_transferred);

public:
    explicit Socket_API(boost::asio::io_service &socket);

    void set_socket(boost::asio::ip::tcp::socket &&socket);

    boost::asio::ip::tcp::socket &&get_socket();

    template<typename Handler>
    void send(Message *message = new Message{ERROR}, Handler handler = generic_handler);

    template<typename Handler>
    void async_send(Message *message = new Message{ERROR}, Handler handler = generic_handler);

    template<typename Handler>
    Message *receive(MESSAGE_TYPE expectedMessage = UNDEFINED, Handler handler = generic_handler);


    ~Socket_API();
};


#endif //SERVER_SOCKET_API_H
