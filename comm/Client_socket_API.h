//
// Created by Paolo Gastaldi on 26/11/2020.
//

#ifndef PDS_PROGETTO_CLIENT_SOCKET_API_H
#define PDS_PROGETTO_CLIENT_SOCKET_API_H

#include <boost/asio/ip/tcp.hpp>
#include "Socket_API.h"

class Client_socket_API : public Socket_API {

public:
    /**
     * class constructor
     * @param socket
     */
    explicit Client_socket_API(boost::asio::ip::tcp::socket &&socket) : Socket_API(std::move(socket)) {}
};


#endif //PDS_PROGETTO_CLIENT_SOCKET_API_H
