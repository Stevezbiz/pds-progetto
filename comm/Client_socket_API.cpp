//
// Created by Paolo Gastaldi on 07/12/2020.
//

#include "Client_socket_API.h"

Client_socket_API::Client_socket_API(std::string ip, std::string port, bool keep_alive) :
        Socket_API(std::move(ip), std::move(port), RETRY_ONCE, 500, keep_alive) {}

bool Client_socket_API::send(Message *message) {
    Socket_API::open_conn();
    message->cookie = this->cookie_;
    auto ret_val = Socket_API::send(message);
    return ret_val;
}

bool Client_socket_API::receive(MESSAGE_TYPE expected_message) {
    auto ret_val = Socket_API::receive(expected_message);
    this->cookie_ = Socket_API::get_message()->cookie;
    Socket_API::close_conn();
    return ret_val;
}
