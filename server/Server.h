//
// Created by stevezbiz on 07/11/20.
//

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include "Session.h"
#include "../comm/Server_API.h"
#include "Database_API.h"

constexpr int SOCKET_TIMEOUT = 1000 * 10; // 10 sec

class Server {
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;
    Server_API *api_;
    Database_API db_;
    std::string root_path_;
    bool stop_;
    Session_manager *session_manager_{};

    /**
     * Accetta le connessioni dei client
     */
    void accept();

    /**
     *
     * @return
     */
    static bool login(Session *, const std::string &, const std::string &, const Database_API &database);

    /**
     *
     * @return
     */
    static const std::unordered_map<std::string, std::string> *probe(Session *, const std::vector<std::string> &);

    /**
     *
     * @return
     */
    static const std::vector<unsigned char> *get(Session *, const std::string &, const std::string &);

    /**
     *
     */
    static bool push(Session *, const std::string &, const std::vector<unsigned char> &, const std::string &, ElementStatus, const std::string &);

    /**
     *
     */
    static const std::vector<std::string> *restore(Session *);

    /**
     *
     * @return
     */
    static bool end(Session *);

    /**
     * @param comm_error
     */
    static void handle_error(Session *, const Comm_error *);

    /**
     *
     */
    void server_init();

public:

    /**
     * Inizializza i campi dell'oggetto e pone il server in attesa di accettare connessioni
     * @param ctx: i servizi di I/O forniti
     * @param endpoint: l'indirizzo e la porta a cui lanciare il server
     * @param db: database path
     * @param root_path: root directory for saving bakup
     */
    Server(boost::asio::io_context &io_service, const boost::asio::ip::tcp::endpoint &endpoint, std::string db_path,
           std::string root_path);

    ~Server();
};

#endif //SERVER_SERVER_H
