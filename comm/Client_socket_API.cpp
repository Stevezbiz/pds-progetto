//
// Created by Paolo Gastaldi on 07/12/2020.
//

#include "Client_socket_API.h"

Client_socket_API::Client_socket_API(std::string ip, std::string port, bool keep_alive) :
        Socket_API(std::move(ip), std::move(port), RETRY_ONCE, 500, keep_alive) {}

bool Client_socket_API::send(Message *message) {
    Logger::info("Client_socket_API::send", "Sending a message...", PR_VERY_LOW);
    if(!Socket_API::open_conn())
        return false;
    message->cookie = this->cookie_;
    auto ret_val = Socket_API::send(message);
    Logger::info("Client_socket_API::send", "Sending a message... - done", PR_VERY_LOW);
    return ret_val;
}

bool Client_socket_API::receive(MESSAGE_TYPE expected_message) {
    Logger::info("Client_socket_API::receive", "Receiving a message...", PR_VERY_LOW);
    auto ret_val = Socket_API::receive(expected_message);
    this->cookie_ = Socket_API::get_message()->cookie;
    if(!Socket_API::close_conn())
        return false;
    Logger::info("Client_socket_API::receive", "Receiving a message... -done", PR_VERY_LOW);
    return ret_val;
}

bool Client_socket_API::send_and_receive(Message *message, MESSAGE_TYPE expected_message) {
    Logger::info("Socket_API::send_and_receive", "Sending and receiving...", PR_VERY_LOW);
    if(!this->send(message))
        return false;
    if(!this->receive(expected_message))
        return false;
    Logger::info("Socket_API::send_and_receive", "Sending and receiving... - done", PR_VERY_LOW);
    return true;
}