//
// Created by stevezbiz on 07/11/20.
//

#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H

#include <boost/asio.hpp>
#include <deque>
#include <iostream>
#include <sqlite3.h>
#include "../comm/Server_API.h"

class Session : public std::enable_shared_from_this<Session> {
    std::string user_;
    Server_API *api_;

    /**
     * Autentica il client tramite la verifica di username e password
     */
    void authenticate_client();

    /**
     * Esamina il database per verificare le credenziali fornite dall'utente
     * @param username: lo username digitata dall'utente
     * @param password: la password digitata dall'utente
     * @return: true se l'autenticazione è andata a buon fine
     */
    bool do_authenticate(std::string username, const std::string& password);

public:
    /**
     * Inizializza i campi dell'oggetto
     * @param socket: socket su cui aprire la connessione
     */
    Session(boost::asio::ip::tcp::socket socket);

    /**
     * Dopo l'autenticazione del client pone il server in ascolto di messaggi in arrivo dal client
     */
    void start();
};

#endif //SERVER_SESSION_H
