//
// Created by stevezbiz on 07/11/20.
//

#include "Server.h"

/**
 * Inizializza i campi dell'oggetto e pone il server in attesa di accettare connessioni
 * @param ctx: i servizi di I/O forniti
 * @param endpoint: l'indirizzo e la porta a cui lanciare il server
 */
Server::Server(boost::asio::io_context &ctx, const boost::asio::ip::tcp::endpoint &endpoint)
        : acceptor_(ctx, endpoint), socket_(ctx) {
    do_accept();
}

/**
 * Accetta le connessioni dei client
 */
void Server::do_accept() {
    acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
        if (!ec) {
            std::make_shared<Session>(std::move(socket_))->start();
        }
        do_accept();
    });
}