//
// Created by Paolo Gastaldi on 23/11/2020.
//

#ifndef SERVER_CLIENT_API_H
#define SERVER_CLIENT_API_H

#include <iostream>
#include <vector>
#include <functional>
#include <boost/filesystem.hpp>
#include "API.h"
#include "Socket_API.h"
#include "Utils.h"

namespace fs = boost::filesystem;

/**
 * easy class to manage client-side protocol usage
 */
class Client_API : public API {
    /**
     * get a file from the server and save it
     * @param path
     * @return status
     */
    bool get_and_save_(const std::string &path);

public:

    /**
     * class constructor
     * @param socket_api
     * @param root_path
     */
    explicit Client_API(Socket_API *socket_api, const std::string &root_path = ".");

    /**
     * do the login complete procedure:
     * 1- send username and password
     * 2- receive server approval
     *
     * @param username
     * @param password
     * @return status
     */
    bool login(const std::string &username, const std::string &password);

    /**
     * do the complete probe procedure:
     * 1- send a list of paths to the server
     * 2- receive a map <path, hash>
     * 3- compare the hash for each file and push all changes
     *
     * @param map - <path, hash> mapping
     * @return status
     */
    bool probe(const std::map <std::string, std::string> &map);

    /**
     * do the complete push procedure:
     * 1- send file, path and hash to the server
     * 2- receive server approval
     *
     * @param file
     * @param path
     * @param hash
     * @return status
     */
    bool push(const std::vector<unsigned char> &file, const std::string &path, const std::string &hash);

    /**
     * do the complete restore procedure:
     * 1- send request to the server
     * 2- receive the list of all files
     * 3- for each file, get and save it
     *
     * @return status
     */
    bool restore();

    /**
     * do the end procedure:
     * 1- send request to the server
     * 2- receive the server approval
     *
     * @return status
     */
    bool end();
};


#endif //SERVER_CLIENT_API_H
