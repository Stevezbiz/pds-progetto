//
// Created by stevezbiz on 26/11/20.
//

#include "Server_API.h"

/**
     * manage login protocol procedure
     * @param request message
     * @return response message
     */
Message *Server_API::do_login(Message *req) {
    auto status = this->login(req->username, req->password);
    return status ? Message::okay() : Message::error();
}

/**
 * manage probe protocol procedure
 * @param request message
 * @return response message
 */
Message *Server_API::do_probe(Message *req) {
    auto hashes = this->probe(req->paths);
    return Message::probe_content(hashes);
}

/**
 * manage get protocol procedure
 * @param request message
 * @return response message
 */
Message *Server_API::do_get(Message *req) {
    auto file = this->get(req->path);
    return Message::get_content(file, req->path);
}

/**
 * manage push protocol procedure
 * @param request message
 * @return response message
 */
Message *Server_API::do_push(Message *req) {
    auto status = this->push(req->path, req->file, req->hash);
    return status ? Message::okay() : Message::error();
}

/**
 * manage restore protocol procedure
 * @param request message
 * @return response message
 */
Message *Server_API::do_restore(Message *req) {
    auto paths = this->restore();
    return Message::restore_content(paths);
}

/**
     * class constructor
     * @param socket_api
     * @param user_root_path
     */
explicit Server_API::Server_API(Socket_API *socket_api, const std::string &user_root_path = ".") : API(socket_api,
                                                                                                       user_root_path) {}

/**
 * set how to manage a login request
 * @param login_function
 * - input:
 *      - const std::string &username
 *      - const std::string &password
 * - output: bool is_okay
 */
void Server_API::set_login(const std::function<bool(const std::string &, const std::string &)> &login_function) {
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
void Server_API::set_probe(const std::function<const std::map <std::string, std::string> &(
        const std::vector <std::string> &)> &probe_function) {
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
void Server_API::set_get(const std::function<const std::vector<unsigned char> &(const std::string &)> &get_function) {
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
void Server_API::set_push(const std::function<bool(const std::string &, const std::vector<unsigned char> &,
                                                   const std::string &)> &push_function) {
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
void Server_API::set_restore(const std::function<const std::vector <std::string> &()> &restore_function) {
    this->restore = restore_function;
}

/**
 * start handling requests
 * @tparam Handler
 * @param handler
 */
template<typename Handler>
void Server_API::run(Handler handler) {
    while (true) {
        auto req = this->api->receive(new Message{ERROR}, handler);
        Message *res;

        // manage the request and produce a response message
        switch (req->code) {
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
            default:
                res = new Message{ERROR};
        }
        this->api->async_send(res, handler);
    }
}