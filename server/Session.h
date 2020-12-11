//
// Created by stevezbiz on 07/11/20.
//

#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H

#include <boost/asio.hpp>
#include <iostream>
#include <unordered_map>
#include "../comm/Socket_API.h"
#include "Database_API.h"

class Session {

public:
    int session_id{ -1 };
    std::string user;
    bool login_status{ false };

    /**
     * Inizializza i campi dell'oggetto
     * @param socket: socket su cui aprire la connessione
     */
    Session(int session_id);

    /**
     * genereate a cookie to identify this session
     * @return cookie
     */
    std::string get_cookie() const;

    /**
     * check if login has been performed
     * @return login status
     */
    bool is_logged_in() const;

    /**
     *
     * @param path
     * @param hash
     * @return
     */
    bool create_file(const Database_API &database, const std::string &path, const std::string &hash) const;

    /**
     *
     * @param path
     * @param hash
     * @return
     */
    static bool modify_file(const Database_API &database, const std::string &path, const std::string &hash);

    /**
     *
     * @param path
     * @return
     */
    static bool remove_file(const Database_API &database, const std::string &path);

    /**
     *
     * @return
     */
    [[nodiscard]] const std::vector<std::string> *get_path_schema(const Database_API &database) const;

    /**
     *
     */
    [[nodiscard]] const std::unordered_map<std::string, std::string> *get_schema(const Database_API &database) const;

    /**
     *
     * @param path
     * @param hash
     * @return
     */
    [[nodiscard]] bool create_dir(const Database_API &database, const std::string &path, const std::string &hash) const;

    /**
     *
     * @param path
     * @return
     */
    static bool remove_dir(const Database_API &database, const std::string &path);
};

#endif //SERVER_SESSION_H
