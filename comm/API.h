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

namespace fs = boost::filesystem;

class API {

protected:
    Socket_API &api;
    boost::filesystem::path root_path;

    explicit API(Socket_API &socket_api, const std::string &root_path = ".");

    bool _save_file(Message &message);
};


#endif //SERVER_API_H