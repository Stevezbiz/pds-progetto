//
// Created by stevezbiz on 07/11/20.
//

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include "Session.h"

class Server {
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;

    /**
     * Accetta le connessioni dei client
     */
    void do_accept();

public:
    /**
     * Inizializza i campi dell'oggetto e pone il server in attesa di accettare connessioni
     * @param ctx: i servizi di I/O forniti
     * @param endpoint: l'indirizzo e la porta a cui lanciare il server
     */
    Server(boost::asio::io_context &io_service, const boost::asio::ip::tcp::endpoint &endpoint);
};

#endif //SERVER_SERVER_H
