//
// Created by stevezbiz on 26/11/20.
//

#include "Client_API.h"

namespace fs = boost::filesystem;

/**
     * get a file from the server and save it
     * @tparam Handler
     * @param path
     * @param handler
     * @return status
     */
template<typename Handler>
bool Client_API::_get_and_save(const std::string &path, Handler handler) {
    auto req = Message::get(path);
    this->api.async_send(req, handler);
    auto res = this->api.receive(new Message{GET_CONTENT}, handler);
    if (!res->is_okay())
        return false;

    if (!this->_save_file(res))
        return false;
}

/**
     * class constructor
     * @param socket_api
     * @param root_path
     */
Client_API::Client_API(Socket_API &socket_api, const std::string &root_path) : API(socket_api, root_path) {}

/**
     * do the login complete procedure:
     * 1- send username and password
     * 2- receive server approval
     *
     * @tparam Handler
     * @param username
     * @param password
     * @param handler
     * @return status
     */
template<typename Handler>
bool Client_API::do_login(const std::string &username, const std::string &password, Handler handler) {
    auto req = Message::login(username, password);
    this->api.async_send(req, handler);
    auto res = this->api.receive(new Message{OKAY}, handler);

    return res->is_okay();
}

/**
     * do the complete probe procedure:
     * 1- send a list of paths to the server
     * 2- receive a map <path, hash>
     * 3- compare the hash for each file and push all changes
     *
     * @tparam Handler
     * @param map - <path, hash> mapping
     * @param handler
     * @return status
     */
template<typename Handler>
bool Client_API::do_probe(const std::map<std::string, std::string> &map, Handler handler) {
    std::vector<std::string> paths;
    paths.reserve(map.size());
    for (auto const &item : map)
        paths.push_back(item.first);
    auto req = Message::probe(paths);
    this->api.async_send(req, handler);
    auto res = this->api.receive(new Message{PROBE_CONTENT}, handler);

    if (!res->is_okay())
        return false;

    for (auto const &item : res->hashes) {
        auto it = map.find(item.first);
        if (it == map.end() ||
            map[item.first] != item.second) { // check if the file not exists or if versions (hashes) are different
            // if the client has a different file than the server, the client starts a push procedure
            auto path = item.first;
            fs::ifstream is{path};
            if (!this->do_push(path, is, map[path], handler)) {
                is.close();
                return false;
            }

            is.close();
        }
    }
}

/**
     * do the complete push procedure:
     * 1- send file, path and hash to the server
     * 2- receive server approval
     *
     * @tparam Handler
     * @param file
     * @param path
     * @param hash
     * @param handler
     * @return status
     */
template<typename Handler>
bool Client_API::do_push(const std::vector<unsigned char> &file, const std::string &path, const std::string &hash,
                         Handler handler) {
    auto req = Message::push(file, path, hash);
    this->api.async_send(req, handler);
    auto res = this->api.receive(new Message{OKAY}, handler);

    return res->is_okay();
}

/**
     * do the complete restore procedure:
     * 1- send request to the server
     * 2- receive the list of all files
     * 3- for each file, get and save it
     *
     * @tparam Handler
     * @param handler
     * @return status
     */
template<typename Handler>
bool Client_API::do_restore(Handler handler) {
    auto req = Message::restore();
    this->api.async_send(req, handler);
    auto res = this->api.receive(new Message{RESTORE_CONTENT}, handler);

    if (!res->is_okay())
        return false;

    for (auto path : res->paths)
        if (!this->_get_and_save(path, handler))
            return false;

    return true;
}