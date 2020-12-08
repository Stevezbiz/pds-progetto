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
    int n_retry_;
    long retry_delay_;
    bool keep_alive_;

protected:
    boost::asio::ip::tcp::socket *socket_ = nullptr;

    /**
     * class constructor
     */
    Socket_API() = default;

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
    std::string ip;
    std::string port;
    Message *message = nullptr;
    Comm_error *comm_error = nullptr;

    /**
     * class constructor
     * @param ip
     * @param port
     * @param error_management
     * @param retry_delay
     * @param keep_alive
     */
    explicit Socket_API(std::string ip, std::string port, ERROR_MANAGEMENT error_management = NO_RETRY, long retry_delay = 1000, bool keep_alive = true);

    /**
     * class constructor
     * @param socket
     * @param error_management
     * @param retry_delay
     * @param keep_alive
     */
    explicit Socket_API(boost::asio::ip::tcp::socket socket, ERROR_MANAGEMENT error_management = NO_RETRY, long retry_delay = 1000, bool keep_alive = true);

    /**
     * open a connection towards the specified ip and port
     * @return status
     */
    bool open_conn();

    /**
     * send a Message
     * (sync mode by default)
     * wait until the message as been sent
     * @param message
     * @return status
     */
    bool send(Message *message = new Message{MSG_ERROR });

    /**
     * receive a message
     * @param expectedMessage
     * @return status
     */
    bool receive(MESSAGE_TYPE expectedMessage = MSG_UNDEFINED);

    /**
     * close current connection
     * @return status
     */
    bool close_conn();

    /**
     * send a message and retrieve the response
     * @param message
     * @param expected_message
     * @return status
     */
    bool send_and_receive(Message *message, MESSAGE_TYPE expected_message);

    /**
     * getter
     * @return last message
     */
    Message *get_message() const;

    /**
     * getter
     * @return last error
     */
    Comm_error *get_last_error() const;

    ~Socket_API();
};


#endif //SERVER_SOCKET_API_H
