//
// Created by Paolo Gastaldi on 26/11/2020.
//

#ifndef PDS_PROGETTO_CLIENT_SOCKET_API_H
#define PDS_PROGETTO_CLIENT_SOCKET_API_H

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/system/system_error.hpp>
#include <utility>
#include "Socket_API.h"

class Client_socket_API : public Socket_API {
    std::string cookie_ = "";

    /**
     * check if this is a connection error status
     * @return status
     */
    bool is_connection_error_();

public:
    /**
     * class constructor
     * @param ip
     * @param port
     * @param keep_alive
     */
    Client_socket_API(std::string ip, std::string port, bool keep_alive = true);

    /**
     * send a message
     * @param message
     * @return status
     */
    bool open_and_send(Message *message);

    /**
     * receive a message
     * @param expected_message
     * @return status
     */
    bool receive_and_close(MESSAGE_TYPE expected_message);

    /**
     * send a message and retrieve the response
     * @param message
     * @param expected_message
     * @return status
     */
    bool send_and_receive(Message *message, MESSAGE_TYPE expected_message);
};


#endif //PDS_PROGETTO_CLIENT_SOCKET_API_H
