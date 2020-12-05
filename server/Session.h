//
// Created by stevezbiz on 07/11/20.
//

#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H

#include <boost/asio.hpp>
#include <deque>
#include <iostream>
#include <sqlite3.h>
#include "../comm/Socket_API.h"

class Session : public std::enable_shared_from_this<Session> {
    std::string user_;
    Socket_API api_;

public:
    /**
     * Inizializza i campi dell'oggetto
     * @param socket: socket su cui aprire la connessione
     */
    Session(boost::asio::ip::tcp::socket socket);

    /**
     *
     * @param user
     */
    void set_user(std::string user);

    /**
     *
     * @param expectedMessage
     * @return
     */
    bool receive(MESSAGE_TYPE expectedMessage);

    /**
     *
     * @return
     */
    Comm_error *get_last_error();

    /**
     *
     * @param message
     * @return
     */
    bool send(Message *message);

    /**
     *
     * @return
     */
    Message *get_message();

    /**
     *
     * @return
     */
    std::string get_user();
};

#endif //SERVER_SESSION_H
