//
// Created by Paolo Gastaldi on 21/11/2020.
//

#ifndef SERVER_SOCKET_API_H
#define SERVER_SOCKET_API_H

#include "Message.h"

class Socket_API {
    boost::asio::ip::tcp::socket socket_;

    static void generic_handler(const boost::system::error_code &ec, std::size_t bytes_transferred);

public:
    explicit Socket_API(boost::asio::io_service &socket);

    void set_socket(boost::asio::ip::tcp::socket &&socket);

    boost::asio::ip::tcp::socket &&get_socket();

    void send(Message *message = new Message{ ERROR }, Handler handler = generic_handler);

    /**
     * receive a message
     * @tparam Handler
     * @param expectedMessage
     * @param handler
     * @return message
     */
    template <typename Handler>
    Message *receive(MESSAGE_TYPE expectedMessage = UNDEFINED, Handler handler = generic_handler) {
        auto message = new Message{};
        boost::asio::read(this->socket_, message->get_header_buffer(), handler);

        message->build(); // build the header
        if(expectedMessage != UNDEFINED && message->code != expectedMessage)
            return new Message{ ERROR };

    ~Socket_API();
};


#endif //SERVER_SOCKET_API_H
