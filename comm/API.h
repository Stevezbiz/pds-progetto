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
#include "Comm_error.h"
#include "Utils.h"

namespace fs = boost::filesystem;

class API {

protected:
    Socket_API *api;
    boost::filesystem::path root_path;

    /**
     * class constructor
     * @param socket_api
     * @param root_path
     */
    explicit API(Socket_API *socket_api, const std::string &root_path = ".");

    /**
     * save a file on disk
     * the path is a relative path from the root_path
     * @param message
     * @return status
     */
    bool save_file_(Message *message);

public:
    /**
     * getter
     * @return message
     */
    Message *get_message();

    /**
     * getter
     * @return last error
     */
    Comm_error *get_last_error();

    ~API();
};


#endif //SERVER_API_H