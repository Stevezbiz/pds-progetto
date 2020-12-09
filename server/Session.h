//
// Created by stevezbiz on 07/11/20.
//

#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H

#include <boost/asio.hpp>
#include <deque>
#include <utility>
#include <iostream>
#include <sqlite3.h>
#include <unordered_map>
#include "../comm/Socket_API.h"
#include "Database_API.h"

class Session : public std::enable_shared_from_this<Session> {
    std::string user_;
    Socket_API api_;
    Database_API &database_;

public:
    /**
     * Inizializza i campi dell'oggetto
     * @param socket: socket su cui aprire la connessione
     */
    Session(boost::asio::ip::tcp::socket socket, Database_API &database);

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

    /**
     *
     * @param path
     * @param hash
     * @return
     */
    bool create_file(const std::string &path, const std::string &hash);

    /**
     *
     * @param path
     * @param hash
     * @return
     */
    bool modify_file(const std::string &path, const std::string &hash);

    /**
     *
     * @param path
     * @return
     */
    bool remove_file(const std::string &path);

    /**
     *
     * @return
     */
    const std::vector<std::string> *get_path_schema();

    /**
     *
     */
    const std::unordered_map<std::string, std::string> *get_schema();

    /**
     *
     * @param path
     * @param hash
     * @return
     */
    bool create_dir(const std::string &path, const std::string &hash);

    /**
     *
     * @param path
     * @return
     */
    bool remove_dir(const std::string &path);
};

#endif //SERVER_SESSION_H
