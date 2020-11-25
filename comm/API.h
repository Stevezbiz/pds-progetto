//
// Created by Paolo Gastaldi on 24/11/2020.
//

#ifndef SERVER_API_H
#define SERVER_API_H

#include <iostream>
#include <utility>
#include <vector>
#include <functional>
#include <boost/filesystem.hpp>
#include "Socket_API.h"

namespace fs = boost::filesystem;

class API {

protected:
    Socket_API *api = nullptr;
    fs::path root_path;

    /**
     * class constructor
     * @param socket_api
     * @param root_path
     */
    explicit API(Socket_API *socket_api, const std::string &root_path = ".") : api(api), root_path(root_path) {}

    /**
     * save a file on disk
     * the path is a relative path from the root_path
     * @param message
     * @return status
     */
    bool _save_file(Message *message) {
        fs::path full_path = this->root_path / fs::path(message->path);
        fs::ofstream os{full_path};

        char *file_buffer = reinterpret_cast<char *>(&(message->file[0]));
        os << file_buffer;
        os.close();

        return true;
    }

    ~API() {
        delete this->api;
    }
};


#endif //SERVER_API_H
