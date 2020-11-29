//
// Created by Paolo Gastaldi on 24/11/2020.
//

#ifndef SERVER_SERVER_API_H
#define SERVER_SERVER_API_H

#include <iostream>
#include <utility>
#include <vector>
#include <functional>
#include <boost/filesystem.hpp>
#include "API.h"
#include "Socket_API.h"
#include "Message.h"
#include "Comm_error.h"

namespace fs = boost::filesystem;

/**
 * easy class to manage server-side protocol usage
 */
class Server_API : public API {
    // functions this class needs to manage client responses
    static std::function<bool(const std::string &, const std::string &)> login;
    static std::function<const std::map <std::string, std::string> *(const std::vector <std::string> &)> probe;
    static std::function<const std::vector<unsigned char> *(const std::string &)> get;
    static std::function<bool(const std::string &, const std::vector<unsigned char> &, const std::string &)> push;
    static std::function<const std::vector <std::string> *()> restore;
    static std::function<bool()> end;

    /**
     * manage login protocol procedure
     * @param request message
     * @return response message
     */
    static Message *do_login(Message *req);

    /**
     * manage probe protocol procedure
     * @param request message
     * @return response message
     */
    static Message *do_probe(Message *req);

    /**
     * manage get protocol procedure
     * @param request message
     * @return response message
     */
    static Message *do_get(Message *req);

    /**
     * manage push protocol procedure
     * @param request message
     * @return response message
     */
    static Message *do_push(Message *req);

    /**
     * manage restore protocol procedure
     * @param request message
     * @return response message
     */
    static Message *do_restore(Message *req);

    /**
    * manage end protocol procedure
    * @param request message
    * @return response message
    */
    static Message *do_end(Message *req);

public:
    /**
     * class constructor
     * @param socket_api
     * @param user_root_path
     */
    explicit Server_API(Socket_API *socket_api, const std::string &user_root_path = ".");

    /**
     * set how to manage a login request
     * @param login_function
     * - input:
     *      - const std::string &username
     *      - const std::string &password
     * - output: bool is_okay
     */
    static void set_login(const std::function<bool(const std::string &, const std::string &)> &login_function);

    /**
     * set how to manage a probe request
     * @param probe_function
     * - input:
     *      - const std::vector<std::string> &, a list of paths
     * - output:
     *      - const std::map<std::string, std::string> &, a map of <path, hash>
     */
    static void set_probe(const std::function<const std::map <std::string, std::string> *( const std::vector <std::string> &)> &probe_function);

    /**
     * set how to manage a get request
     * @param get_function
     * - input:
     *      - const std::string &path
     * - output:
     *      - const std::vector<unsigned char> &file, a vector of bytes
     */
    static void set_get(const std::function<const std::vector<unsigned char> *(const std::string &)> &get_function);

    /**
     * set how to manage a push request
     * @param push_function
     * - input:
     *      - const std::string &path
     *      - const std::vector<char> &file
     *      - const std::string &hash
     * - output:
     *      - bool is_okay
     */
    static void set_push(const std::function<bool(const std::string &, const std::vector<unsigned char> &, const std::string &)> &push_function);

    /**
     * set how to manage a restore request
     * @param restore_function
     * - input:
     *      - (empty)
     * - output:
     *      - const std::vector<std::string> &paths, a list of paths
     */
    static void set_restore(const std::function<const std::vector<std::string> *()> &restore_function);

    /**
     * set how to manage a end request
     * @param restore_function
     * - input:
     *      - (empty)
     * - output:
     *      - bool status
     */
    static void set_end(const std::function<bool()> &end_function);

    /**
     * start handling requests
     */
    void run();
};

// default functions
std::function<bool(const std::string &, const std::string &)> Server_API::login = [](const std::string &, const std::string &) {
    return true;
};

std::function<const std::map<std::string, std::string> *(const std::vector <std::string> &)> Server_API::probe = [](const std::vector <std::string> &) {
    return new std::map<std::string, std::string>{};
};

std::function<const std::vector<unsigned char> *(const std::string &)> Server_API::get = [](const std::string &) {
    return new std::vector<unsigned char>{};
};

std::function<bool(const std::string &, const std::vector<unsigned char> &, const std::string &)> Server_API::push = [](const std::string &, const std::vector<unsigned char> &, const std::string &) {
    return true;
};

std::function<const std::vector<std::string> *()> Server_API::restore = []() {
    return new std::vector<std::string>{};
};

std::function<bool()> Server_API::end = []() {
    return true;
};

#endif //SERVER_SERVER_API_H
