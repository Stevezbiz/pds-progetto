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

namespace fs = boost::filesystem;

/**
 * easy class to manage server-side protocol usage
 */
class Server_API : public API {
    // functions this class needs to manage client responses
    std::function<bool(const std::string &, const std::string &)> login;
    std::function<const std::map<std::string, std::string> &(const std::vector<std::string> &)> probe;
    std::function<const std::vector<unsigned char> &(const std::string &)> get;
    std::function<bool(const std::string &, const std::vector<unsigned char> &, const std::string &)> push;
    std::function<const std::vector<std::string> &()> restore;
    std::function<bool()> end;

    /**
     * manage login protocol procedure
     * @param request message
     * @return response message
     */
    Message *do_login(Message *req) {
        auto status = this->login(req->username, req->password);
        return status ? Message::okay() : Message::error();
    }

    /**
     * manage probe protocol procedure
     * @param request message
     * @return response message
     */
    Message *do_probe(Message *req) {
        auto hashes = this->probe(req->paths);
        return Message::probe_content(hashes);
    }

    /**
     * manage get protocol procedure
     * @param request message
     * @return response message
     */
    Message *do_get(Message *req) {
        auto file = this->get(req->path);
        return Message::get_content(file, req->path);
    }

    /**
     * manage push protocol procedure
     * @param request message
     * @return response message
     */
    Message *do_push(Message *req) {
        auto status = this->push(req->path, req->file, req->hash);
        return status ? Message::okay() : Message::error();
    }

    /**
     * manage restore protocol procedure
     * @param request message
     * @return response message
     */
    Message *do_restore(Message *req) {
        auto paths = this->restore();
        return Message::restore_content(paths);
    }

    /**
     * manage end protocol procedure
     * @param request message
     * @return response message
     */
    Message *do_end(Message *req) {
        auto status = this->end();
        return status ? Message::okay() : Message::error();
    }

public:
    /**
     * class constructor
     * @param socket_api
     * @param user_root_path
     */
    explicit Server_API(Socket_API *socket_api, const std::string &user_root_path = ".") : API(socket_api, user_root_path) {}

    /**
     * set how to manage a login request
     * @param login_function
     * - input:
     *      - const std::string &username
     *      - const std::string &password
     * - output: bool is_okay
     */
    void set_login(const std::function<bool(const std::string &, const std::string &)> &login_function) {
        this->login = login_function;
    }

    /**
     * set how to manage a probe request
     * @param probe_function
     * - input:
     *      - const std::vector<std::string> &, a list of paths
     * - output:
     *      - const std::map<std::string, std::string> &, a map of <path, hash>
     */
    void set_probe(const std::function<const std::map<std::string, std::string> &(const std::vector<std::string> &)> &probe_function) {
        this->probe = probe_function;
    }

    /**
     * set how to manage a get request
     * @param get_function
     * - input:
     *      - const std::string &path
     * - output:
     *      - const std::vector<unsigned char> &file, a vector of bytes
     */
    void set_get(const std::function<const std::vector<unsigned char> &(const std::string &)> &get_function) {
        this->get = get_function;
    }

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
    void set_push(const std::function<bool(const std::string &, const std::vector<unsigned char> &, const std::string &)> &push_function) {
        this->push = push_function;
    }

    /**
     * set how to manage a restore request
     * @param restore_function
     * - input:
     *      - (empty)
     * - output:
     *      - const std::vector<std::string> &paths, a list of paths
     */
    void set_restore(const std::function<const std::vector<std::string> &()> &restore_function) {
        this->restore = restore_function;
    }

    /**
     * set how to manage a end request
     * @param end_function
     * - input:
     *      - (empty)
     * - output:
     *      - bool status
     */
    void set_end(const std::function<bool()> &end_function) {
        this->end = end_function;
    }

    /**
     * start handling requests
     * @tparam Handler
     * @param handler
     */
    template <typename Handler>
    void run(Handler handler) {
        bool stop = false;
        while(!stop) {
            auto req = this->api->receive(UNDEFINED, handler);
            Message *res;

            // manage the request and produce a response message
            switch(req->code) {
                case LOGIN:
                    res = this->do_login(req);
                    break;
                case PROBE:
                    res = this->do_probe(req);
                    break;
                case GET:
                    res = this->do_get(req);
                    break;
                case PUSH:
                    res = this->do_push(req);
                    break;
                case RESTORE:
                    res = this->do_restore(req);
                    break;
                case END:
                    res = this->do_end(req);
                    stop = true;
                    break;
                default:
                    res = new Message{ ERROR };
            }
            this->api->async_send(res, handler);
        }
    }
};


#endif //SERVER_SERVER_API_H
