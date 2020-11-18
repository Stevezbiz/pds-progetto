//
// Created by stevezbiz on 07/11/20.
//

#include <iostream>
#include "Session.h"

using namespace boost::asio;

/**
 * Inizializza i campi dell'oggetto
 * @param socket: socket su cui aprire la connessione
 */
Session::Session(ip::tcp::socket socket) : socket_(std::move(socket)) {}

/**
 * Pone il server in ascolto di messaggi in arrivo dal client
 */
void Session::start() {
    do_read_header();
}

/**
 * Invia un messaggio al client
 * @param msg: messaggio da inviare
 */
void Session::write(const Message &msg) {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress) {
        do_write();
    }
}

/**
 * Legge l'header del messaggio in arrivo, lo decodifica e lancia la lettura del body
 */
void Session::do_read_header() {
    auto self(shared_from_this());
    async_read(socket_, buffer(read_msg_.getData(), Message::header_length),
               [this, self](boost::system::error_code ec, std::size_t length) {
                   if (!ec && read_msg_.decodeHeader()) {
                       do_read_body();
                   } else {

                   }
               });
}

/**
 * Legge il body sulla base delle informazioni presenti nell'header e si pone in ascolto del prossimo header
 */
void Session::do_read_body() {
    auto self(shared_from_this());
    async_read(socket_, buffer(read_msg_.getBody(), read_msg_.getBodyLength()),
               [this, self](boost::system::error_code ec, std::size_t length) {
                   if (!ec) {
                       std::cout << read_msg_.getBody() << std::endl;
                       //write(read_msg_);
                       do_read_header();
                   } else {

                   }
               });
}

/**
 * Esegue l'invio di un messaggio al client
 */
void Session::do_write() {
    auto self(shared_from_this());
    async_write(socket_, buffer(write_msgs_.front().getData(), write_msgs_.front().getLength()),
                [this, self](boost::system::error_code ec, std::size_t length) {
                    if (!ec) {
                        write_msgs_.pop_front();
                        if (!write_msgs_.empty()) {
                            do_write();
                        }
                    } else {

                    }
                });
}
