//
// Created by stevezbiz on 07/11/20.
//

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include "Session.h"
#include "../comm/Server_API.h"
#include "Database_API.h"
#include "Session_manager.h"

constexpr int SOCKET_TIMEOUT = 1000 * 1000; // 10 sec
constexpr int MAX_THREADS = 32;

class Server {
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;
    std::unique_ptr<Server_API> api_;
    Database_API db_;
    std::string root_path_;
    bool stop_;
    std::shared_ptr<Session_manager> session_manager_;
    std::mutex m_threads_;
    std::condition_variable cv_;
    std::atomic<int> n_active_threads_{0};

    /**
     * Accetta le connessioni dei client
     */
    void accept();

    /**
     *
     * @return
     */
    static bool login(Session *, const std::string &, const std::string &, const Database_API &, const std::string &);

    /**
     *
     * @return
     */
    static const std::unordered_map<std::string, std::string> *
    probe(Session *, const std::vector<std::string> &, const Database_API &);

    /**
     *
     * @return
     */
    static const std::vector<unsigned char> *get(Session *, const std::string &, const std::string &, ElementStatus &);

    /**
     *
     */
    static bool push(Session *, const std::string &, const std::vector<unsigned char> &, const std::string &,
                     ElementStatus, const std::string &, const Database_API &);

    /**
     *
     */
    static const std::vector<std::string> *restore(Session *, const Database_API &);

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

    /**
     *
     * @param base
     * @param path
     */
    static void create_dirs(boost::filesystem::path base, const std::string &path);

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
};

#endif //SERVER_SERVER_H
