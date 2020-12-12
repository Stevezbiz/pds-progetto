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
    Socket_API *api_;

    /**
     * class constructor
     * @param socket_api
     */
    explicit API(Socket_API *socket_api = nullptr);

    /**
     * save a file on disk
     * the path is a relative path from the root_path
     * @param message
     * @return status
     */
    static bool save_file_(const std::shared_ptr<Message> &message);

public:
    /**
     * getter
     * @return message
     */
    std::shared_ptr<Message> get_message();

    /**
     * getter
     * @return last error
     */
    std::shared_ptr<Comm_error>  get_last_error();

    ~API();
};


#endif //SERVER_API_H