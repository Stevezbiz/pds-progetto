//
// Created by Paolo Gastaldi on 23/11/2020.
//

#ifndef SERVER_CLIENT_API_H
#define SERVER_CLIENT_API_H

#include <iostream>
#include <utility>
#include <vector>
#include <functional>
#include <boost/filesystem.hpp>
#include "Socket_API.h"

namespace fs = boost::filesystem;

class Client_API {
    Socket_API *api;
    std::function<std::string(std::ifstream &)> hash_function;
    fs::path root_path;

    /**
     * save a file on disk
     * the path is a relative path from the root_path
     * @param message
     * @return status
     */
    bool _save_file(Message *message) {
        fs::path full_path = this->root_path / fs::path(message->path);
        fs::ofstream os{ full_path };

        char *file_buffer = reinterpret_cast<char *>(&(message->file[0]));
        os << file_buffer;
        os.close();

        return true;
    }

    /**
     * get a file from the server and save it
     * @tparam Handler
     * @param path
     * @param handler
     * @return status
     */
    template <typename Handler>
    bool _get_and_save(const std::string &path, Handler handler) {
        auto req = Message::get(path);
        this->api->async_send(req, handler);
        auto res = this->api->receive(new Message{ GET_CONTENT }, handler);
        if(!res->isOkay())
            return false;

        if(!_save_file(res))
            return false;
    }

public:
    explicit Client_API(Socket_API *api, std::function<std::string(std::ifstream &)> hash_function, std::string &&root_path = ".") : api(api), hash_function(std::move(hash_function)), root_path(fs::path{ root_path }) {}

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
    template <typename Handler>
    bool do_login(const std::string &username, const std::string &password, Handler handler) {
        auto req = Message::login(username, password);
        this->api->async_send(req, handler);
        auto res = this->api->receive(new Message{ OKAY }, handler);

        return res->isOkay();
    }

    /**
     * do the complete probe procedure:
     * 1-
     * 2-
     * @tparam Handler
     * @param map - <path, hash> mapping
     * @param handler
     * @return status
     */
    template <typename Handler>
    bool do_probe(const std::map<std::string, std::string> &map, Handler handler) {
        std::vector<std::string> paths;
        paths.reserve(map.size());
        for(auto const &item : map)
            paths.push_back(item.first);
        auto req = Message::probe(paths);
        this->api->async_send(req, handler);
        auto res = this->api->receive(new Message{ PROBE_CONTENT }, handler);

        if(!res->isOkay())
            return false;

        for(auto const &item : res->hashes) {
            if(map[item.first] != item.second) { // check if the version is different
                // if the client has a different file than the server, the client starts a push procedure
                auto path = item.first;
                fs::ifstream is{ path };
                if(!this->do_push(path, is, hash_function(is), handler)) {
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
    template <typename Handler>
    bool do_push(std::istream &file, const std::string &path, const std::string &hash, Handler handler) {
        auto req = Message::push(file, path, hash);
        this->api->async_send(req, handler);
        auto res = this->api->receive(new Message{ OKAY }, handler);

        return res->isOkay();
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
    template <typename Handler>
    bool do_restore(Handler handler) {
        auto req = Message::restore();
        this->api->async_send(req, handler);
        auto res = this->api->receive(new Message{ RESTORE_CONTENT }, handler);

        if(!res->isOkay())
            return false;

        for(auto path : res->paths)
            if(!this->_get_and_save(path, handler))
                return false;

        return true;
    }
};


#endif //SERVER_CLIENT_API_H
