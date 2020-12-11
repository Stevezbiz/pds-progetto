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
#include <future>
#include <chrono>
#include "API.h"
#include "Socket_API.h"
#include "Message.h"
#include "Comm_error.h"
#include "../server/Session.h"
#include "../server/Session_manager.h"

namespace fs = boost::filesystem;

constexpr int DEFAULT_SOCKET_TIMEOUT = 1000 * 30; // 30 seconds

/**
 * easy class to manage server-side protocol usage
 */
class Server_API : public API {
    Session_manager *session_manager_;

    // functions this class needs to manage client responses
    std::function<bool(Session *, const std::string &, const std::string &)> login_ =
            [](Session *, const std::string &, const std::string &) {
                return true;
            };
    std::function<const std::unordered_map<std::string, std::string> *(Session *, const std::vector<std::string> &)> probe_ =
            [](Session *, const std::vector<std::string> &) {
                return new std::unordered_map<std::string, std::string>{};
            };
    std::function<const std::vector<unsigned char> *(Session *, const std::string &)> get_ =
            [](Session *, const std::string &) {
                return new std::vector<unsigned char>{};
            };
    std::function<bool(Session *, const std::string &, const std::vector<unsigned char> &, const std::string &, ElementStatus)> push_ =
            [](Session *, const std::string &, const std::vector<unsigned char> &, const std::string &, ElementStatus) {
                return true;
            };
    std::function<const std::vector<std::string> *(Session *)> restore_ = [](Session *) {
            return new std::vector<std::string>{};
        };
    std::function<bool(Session *)> end_ = [](Session *) {
            return true;
        };
    std::function<void(Session *, const Comm_error *)> handle_error_ = [](Session *, const Comm_error *comm_error) {
            Logger::error(comm_error);
        };

    /**
     * manage login protocol procedure
     * @param session
     * @param request message
     * @return response message
     */
    Message *do_login_(Session *session, Message *req);

    /**
     * manage probe protocol procedure
     * @param session
     * @param request message
     * @return response message
     */
    Message *do_probe_(Session *session, Message *req);

    /**
     * manage get protocol procedure
     * @param session
     * @param request message
     * @return response message
     */
    Message *do_get_(Session *session, Message *req);

    /**
     * manage push protocol procedure
     * @param session
     * @param request message
     * @return response message
     */
    Message *do_push_(Session *session, Message *req);

    /**
     * manage restore protocol procedure
     * @param session
     * @param request message
     * @return response message
     */
    Message *do_restore_(Session *session, Message *req);

    /**
    * manage end protocol procedure
    * @param session
    * @param request message
    * @return response message
    */
    Message *do_end_(Session *session, Message *req);

    /**
     * handle a communication error
     * @param session
     * @param comm_error
     * @return response message
     */
    void do_handle_error_(Session *session, Comm_error *comm_error);

public:
    /**
     * class constructor
     * @param session_manager
     */
    explicit Server_API(Session_manager *session_manager);

    /**
     * set how to manage a login request
     * @param login_function
     * - input:
     *      - Session *session
     *      - const std::string &username
     *      - const std::string &password
     * - output: bool is_okay
     */
    void set_login(const std::function<bool(Session *, const std::string &, const std::string &)> &login_function);

    /**
     * set how to manage a probe request
     * @param probe_function
     * - input:
     *      - Session *session
     *      - const std::vector<std::string> &, a list of paths
     * - output:
     *      - const std::map<std::string, std::string> &, a map of <path, hash>
     */
    void set_probe(const std::function<const std::unordered_map<std::string, std::string> *(Session *, const std::vector<std::string> &)> &probe_function);

    /**
     * set how to manage a get request
     * @param get_function
     * - input:
     *      - Session *session
     *      - const std::string &path
     * - output:
     *      - const std::vector<unsigned char> &file, a vector of bytes
     */
    void set_get(const std::function<const std::vector<unsigned char> *(Session *, const std::string &)> &get_function);

    /**
     * set how to manage a push request
     * @param push_function
     * - input:
     *      - Session *session
     *      - const std::string &path
     *      - const std::vector<char> &file
     *      - const std::string &hash
     *      - ElementStatus
     * - output:
     *      - bool is_okay
     */
    void set_push(const std::function<bool(Session *, const std::string &, const std::vector<unsigned char> &, const std::string &, ElementStatus)> &push_function);

    /**
     * set how to manage a restore request
     * @param restore_function
     * - input:
     *      - Session *session
     * - output:
     *      - const std::vector<std::string> &paths, a list of paths
     */
    void set_restore(const std::function<const std::vector<std::string> *(Session *)> &restore_function);

    /**
     * set how to manage a end request
     * @param restore_function
     * - input:
     *      - Session *session
     * - output:
     *      - bool status
     */
    void set_end(const std::function<bool(Session *)> &end_function);

    /**
     *
     * @param end_function
     * - input:
     *      - const Comm_error *communication error
     * - output:
     *      - (empty)
     */
    void set_handle_error(const std::function<void(Session *, const Comm_error *)> &handle_error_function);

    /**
     * start handling requests
     * @param api
     * @param socket_timeout
     * @return status
     */
    bool run(Socket_API *api, int socket_timeout = DEFAULT_SOCKET_TIMEOUT);
};

#endif //SERVER_SERVER_API_H
