//
// Created by stevezbiz on 07/11/20.
//

#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H

#include "../comm/Message.h"
#include <boost/asio.hpp>
#include <deque>

class Session : public std::enable_shared_from_this<Session> {
    boost::asio::ip::tcp::socket socket_;
    Message read_msg_;
    std::deque<Message> write_msgs_;
    std::string user_;

    /**
     * Autentica il client tramite la verifica di username e password
     */
    void authenticate_client();

    /**
     * Legge l'header del messaggio in arrivo, lo decodifica e lancia la lettura del body
     */
    void do_read_header();

    /**
     * Legge il body sulla base delle informazioni presenti nell'header e si pone in ascolto del prossimo header
     */
    void do_read_body();

    /**
     * Esegue l'invio di un messaggio al client
     */
    void do_write();

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

    /**
     * Invia un messaggio al client
     * @param msg: messaggio da inviare
     */
    void write(const Message &msg);
};

#endif //SERVER_SESSION_H
