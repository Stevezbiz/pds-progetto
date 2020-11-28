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

protected:
    boost::asio::ip::tcp::socket socket_;
    Message *message{};
    Comm_error *comm_error{};
    int n_retry;
    long retry_delay;

    /**
     * (sync) call read/write functions and manage errors
     * @param perform_this
     * @return status
     */
    bool call_(const std::function<void(boost::asio::ip::tcp::socket &, boost::system::error_code &)> &perform_this);

    /**
     * testing handler
     * @deprecated
     * @param ec
     * @param bytes_transferred
     * @return status
     */
    static bool generic_handler_(const boost::system::error_code& ec, std::size_t bytes_transferred);

    /**
     * receive the packet header
     * @return status
     */
    bool receive_header_();

    /**
     * receive the packet content
     * @return status
     */
    bool receive_content_();

public:
    /**
     * class constructor
     * @param socket
     * @param error_management
     * @param retry_delay
     */
    explicit Socket_API(boost::asio::ip::tcp::socket &&socket, ERROR_MANAGEMENT error_management = NO_RETRY, long retry_delay = 1000);

    /**
     * socket setter
     * @param socket
     */
    void set_socket(boost::asio::ip::tcp::socket &&socket);

    /**
     * socket getter
     * @return socket
     */
    boost::asio::ip::tcp::socket &&get_socket();

    /**
     * send a Message
     * (sync mode by default)
     * wait until the message as been sent
     * @param message
     * @return status
     */
    bool send(Message *message = new Message{ ERROR });

    /**
     * receive a message
     * @param expectedMessage
     * @return status
     */
    bool receive(MESSAGE_TYPE expectedMessage = UNDEFINED);

    /**
     * getter
     * @return last message
     */
    Message *get_message();

    /**
     * getter
     * @return last error
     */
    Comm_error *get_last_error();

    ~Socket_API();
};


#endif //SERVER_SOCKET_API_H
