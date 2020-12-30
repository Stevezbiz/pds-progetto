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
constexpr int MAX_THREADS = 1;

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
     * Accepts incoming connections
     */
    void accept();

    /**
     * Performs the login of a user
     * @return true if login procedure seccessed, false otherwise
     */
    static bool login(Session *, const std::string &, const std::string &, const Database_API &, const std::string &);

    /**
     * Performs the probe requested by a client session
     * @return a map containing the paths and the hashes of the elements already stored on server
     */
    static const std::unordered_map<std::string, std::string> *
    probe(Session *, const std::vector<std::string> &, const Database_API &);

    /**
     * Performs the get requested by a client session
     * @return the content of the file requested by the user
     */
    static const std::vector<unsigned char> *get(Session *, const std::string &, const std::string &, ElementStatus &);

    /**
     * Performs the push requested by a client session
     * @return true if push has been successful, false otherwise
     */
    static bool push(Session *, const std::string &, const std::vector<unsigned char> &, const std::string &,
                     ElementStatus, const std::string &, const Database_API &);

    /**
     * Performs the restore requested by a client session
     * @return a vector containing all the elements stored on server
     */
    static const std::vector<std::string> *restore(Session *, const Database_API &);

    /**
     * Closes a session
     * @return true if the session has been closed correctly, false otherwise
     */
    static bool end(Session *);

    /**
     * Error handler
     * @param comm_error
     */
    static void handle_error(Session *, const Comm_error *);

    /**
     * Initializes the server's API
     */
    void server_init();

    /**
     * Creates all the directories of a path if they do not exist already
     * @param base
     * @param path
     */
    static void create_dirs(boost::filesystem::path base, const std::string &path);

public:

    /**
     * Initializes the object's structures and let the server wait for incoming connections
     * @param ctx
     * @param endpoint: address and port where the server is listening
     * @param db: database path
     * @param root_path: root directory for saving data
     */
    Server(boost::asio::io_context &io_service, const boost::asio::ip::tcp::endpoint &endpoint, std::string db_path,
           std::string root_path);
};

#endif //SERVER_SERVER_H
