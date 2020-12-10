//
// Created by Paolo Gastaldi on 07/12/2020.
//

#include "Client_socket_API.h"

bool Client_socket_API::is_connection_error_() {
    Logger::info("Client_socket_API::is_connection_error_", this->get_last_error()->original_ec.message(), PR_VERY_LOW);
    std::vector errors{ // possible errors
        boost::asio::error::not_connected,
        boost::asio::error::timed_out,
        boost::asio::error::shut_down,
        boost::asio::error::fault
    };
    return std::any_of(errors.begin(), errors.end(), [this](auto error) { return this->get_last_error()->original_ec == error; });
}

Client_socket_API::Client_socket_API(std::string ip, std::string port, bool keep_alive) :
        Socket_API(std::move(ip), std::move(port), RETRY_ONCE, 500, keep_alive) {}

bool Client_socket_API::send_and_receive(Message *message, MESSAGE_TYPE expected_message) {
    Logger::info("Client_socket_API::send_and_receive", "Sending a message...", PR_VERY_LOW);
//    if(!this->open_and_send(message))
//        return false;
//    if(!this->receive_and_close(expected_message))
//        return false;

    if(!this->open_conn())
        return false;
    Logger::info("Client_socket_API::send_and_receive", "Set cookie...", PR_VERY_LOW);
    message->cookie = this->cookie_;
    auto ret_val = this->send(message);

    if(!ret_val) {
        return false;
    }
    Logger::info("Client_socket_API::send_and_receive", "Sending a message... - done", PR_VERY_LOW);

    Logger::info("Client_socket_API::send_and_receive", "Receiving a message...", PR_VERY_LOW);
    ret_val = this->receive(expected_message);
    if(!ret_val) {
        if(this->is_connection_error_()) { // if connection timeout
            Logger::info("Client_socket_API::send_and_receive", "Open the connection again and retry...", PR_VERY_LOW);
            if(!this->open_conn(true))
                return false;
            ret_val = this->send(message);
            if(!ret_val)
                return false;
            ret_val = this->receive(expected_message);
        }
        else
            return false;
    }
    if(ret_val) {
        Logger::info("Client_socket_API::send_and_receive", "Get cookie...", PR_VERY_LOW);
        this->cookie_ = this->get_message()->cookie;
    }

    if(!this->close_conn())
        return false;
    Logger::info("Client_socket_API::send_and_receive", "Receiving a message... - done", PR_VERY_LOW);
    return ret_val;
}
