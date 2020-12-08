//
// Created by Paolo Gastaldi on 26/11/2020.
//

#ifndef PDS_PROGETTO_CLIENT_SOCKET_API_H
#define PDS_PROGETTO_CLIENT_SOCKET_API_H

#include <boost/asio/ip/tcp.hpp>
#include <utility>
#include "Socket_API.h"

class Client_socket_API : public Socket_API {

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
    bool send(Message *message);

    /**
     * receive a message
     * @param expected_message
     * @return status
     */
    bool receive(MESSAGE_TYPE expected_message);
};


#endif //PDS_PROGETTO_CLIENT_SOCKET_API_H
