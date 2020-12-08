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
    std::unordered_map<std::string, std::string> files_;

    /**
     *
     * @param key
     * @return
     */
    bool files_contains(const std::string &key);

public:
    /**
     * Inizializza i campi dell'oggetto
     * @param socket: socket su cui aprire la connessione
     */
    Session(boost::asio::ip::tcp::socket socket);

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
    const std::vector<std::string> *get_paths();

    /**
     *
     */
    const std::unordered_map<std::string, std::string> *get_files();

    void get_path_schema(const Database_API &database);

    bool save_path_schema(const Database_API &database);
};

#endif //SERVER_SESSION_H
