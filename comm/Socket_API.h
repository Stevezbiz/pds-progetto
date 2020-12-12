//
// Created by Paolo Gastaldi on 21/11/2020.
//

#ifndef SERVER_SOCKET_API_H
#define SERVER_SOCKET_API_H

#include "Message.h"
#include "Comm_error.h"

constexpr int CONN_TIMEOUT = 1000 * 10; // 10 seconds

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
    boost::asio::io_context ctx_{};
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator_;

    void init_config_();

protected:
    int n_retry_ = NO_RETRY;
    long retry_delay_ = 0;
    bool keep_alive_ = false;
    std::unique_ptr<boost::asio::ip::tcp::socket> socket_{ nullptr };

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
    static bool generic_handler_(const boost::system::error_code &ec, std::size_t bytes_transferred);

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
    std::shared_ptr<Message>(message);
    std::shared_ptr<Comm_error>(comm_error);

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
     * @param force
     * @return status
     */
    bool open_conn(bool force = false);

    /**
     * send a Message
     * (sync mode by default)
     * wait until the message as been sent
     * @param message
     * @return status
     */
    bool send(std::shared_ptr<Message> message = std::make_shared<Message>(MSG_ERROR));

    /**
     * receive a message
     * @param expectedMessage
     * @return status
     */
    bool receive(MESSAGE_TYPE expectedMessage = MSG_UNDEFINED);

    /**
     * shutdown current connection
     * @return status
     */
    bool shutdown();

    /**
     * close current connection
     * @param force
     * @return status
     */
    bool close_conn(bool force = false);

    /**
     * getter
     * @return last message
     */
    [[nodiscard]] std::shared_ptr<Message> get_message() const;

    /**
     * getter
     * @return last error
     */
    [[nodiscard]] std::shared_ptr<Comm_error> get_last_error() const;

    ~Socket_API();
};


#endif //SERVER_SOCKET_API_H
