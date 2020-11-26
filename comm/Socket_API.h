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

    template<typename Handler>
    void send(Message *message = new Message{ERROR}, Handler handler = generic_handler);

    template<typename Handler>
    void async_send(Message *message = new Message{ERROR}, Handler handler = generic_handler);

    template<typename Handler>
    Message *receive(MESSAGE_TYPE expectedMessage = UNDEFINED, Handler handler = generic_handler);


    ~Socket_API();
};


#endif //SERVER_SOCKET_API_H
