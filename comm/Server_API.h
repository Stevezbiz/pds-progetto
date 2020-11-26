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
#include "API.h"
#include "Socket_API.h"
#include "Message.h"

namespace fs = boost::filesystem;

/**
 * easy class to manage server-side protocol usage
 */
class Server_API : public API {
    // functions this class needs to manage client responses
    std::function<bool(const std::string &, const std::string &)> login;
    std::function<const std::map <std::string, std::string> &(const std::vector <std::string> &)> probe;
    std::function<const std::vector<unsigned char> &(const std::string &)> get;
    std::function<bool(const std::string &, const std::vector<unsigned char> &, const std::string &)> push;
    std::function<const std::vector <std::string> &()> restore;
    std::function<bool()> end;

    Message *do_login(Message *req);

    Message *do_probe(Message *req);

    Message *do_get(Message *req);

    Message *do_push(Message *req);

    Message *do_restore(Message *req);

    Message *do_end(Message *req);

public:
    explicit Server_API(Socket_API &socket_api, const std::string &user_root_path = ".");

    void set_login(const std::function<bool(const std::string &, const std::string &)> &login_function);

    void set_probe(const std::function<const std::map <std::string, std::string> &(
            const std::vector <std::string> &)> &probe_function);

    void set_get(const std::function<const std::vector<unsigned char> &(const std::string &)> &get_function);

    void set_push(const std::function<bool(const std::string &, const std::vector<unsigned char> &,
                                           const std::string &)> &push_function);

    void set_restore(const std::function<const std::vector <std::string> &()> &restore_function);

    template<typename Handler>
    void run(Handler handler);
};


#endif //SERVER_SERVER_API_H
