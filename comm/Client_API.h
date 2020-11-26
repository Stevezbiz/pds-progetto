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
#include "API.h"
#include "Socket_API.h"

/**
 * easy class to manage client-side protocol usage
 */
class Client_API : public API {
    template<typename Handler>
    bool _get_and_save(const std::string &path, Handler handler);

public:
    explicit Client_API(Socket_API &socket_api, const std::string &root_path = ".");

    template<typename Handler>
    bool do_login(const std::string &username, const std::string &password, Handler handler);

    template<typename Handler>
    bool do_probe(const std::map <std::string, std::string> &map, Handler handler);

    template<typename Handler>
    bool do_push(const std::vector<unsigned char> &file, const std::string &path, const std::string &hash, Handler handler);

    template<typename Handler>
    bool do_restore(Handler handler);
};


#endif //SERVER_CLIENT_API_H
