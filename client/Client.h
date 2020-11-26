//
// Created by stevezbiz on 07/11/20.
//

#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include <boost/asio.hpp>
#include <iostream>
#include <deque>
#include "../comm/Message.h"

class Client {
    boost::asio::io_context &ctx_;
    boost::asio::ip::tcp::socket socket_;
    Message read_msg_;
    std::deque<Message> write_msgs_;

    /**
     * Si connette al server e pone il client in ascolto di comunicazioni provenienti dal server
     * @param endpoint_iterator: l'indirizzo risolto del server a cui connettersi
     */
    void do_connect(boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

    /**
     * Legge l'header del messaggio in arrivo, lo decodifica e lancia la lettura del body
     */
    void do_read_header();

    /**
     * Legge il body sulla base delle informazioni presenti nell'header e si pone in ascolto del prossimo header
     */
    void do_read_body();

    /**
     * Esegue l'invio di un messaggio al server
     */
    void do_write();

public:
    /**
     * Inizializza i campi dell'oggetto e lancia la procedura di connessione al server
     * @param ctx: i servizi di I/O forniti
     * @param endpoint_iterator: l'indirizzo risolto del server a cui connettersi
     */
    Client(boost::asio::io_context &ctx, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

    /**
     * Invia un messaggio al server
     * @param msg: il messaggio da inviare al server
     */
    void write(const Message &msg);

    /**
     * Chiude la connessione con il server
     */
    void close();
};


#endif //CLIENT_CLIENT_H
