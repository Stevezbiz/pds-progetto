//
// Created by Paolo Gastaldi on 28/11/2020.
//

#ifndef SERVER_STUB_CLIENT_H
#define SERVER_STUB_CLIENT_H

#include <iostream>
#include <utility>
#include "./FileWatcher.h"
#include "../comm/Client_API.h"
#include "../comm/Client_socket_API.h"

constexpr int FW_DELAY = 1000 * 5; // 5 seconds

class Stub_client {
    FileWatcher *fw;
    Client_API *api;
    bool is_logged_in = false;
    std::map<std::string, std::string> map; // stub map

    /**
     * manage errors
     * @param error
     */
    static void error_handler_(Comm_error *error);

    /**
     * fill stub values
     */
    void prepare_stub();

public:
    /**
     * class constructor
     * @param root_path
     * @param ip
     * @param port
     */
    Stub_client(const std::string &root_path, const std::string &ip, const std::string &port);

    /**
     * perform login actions
     * @param username
     * @param password
     * @return is login performed
     */
    bool login(const std::string &username, const std::string &password);

    /**
     * launch client in normal (default) mode
     * @return is login performed
     */
    bool normal();

    /**
     * launch client in restore mode
     * @return is login performed
     */
    bool restore();

    ~Stub_client();
};


#endif //SERVER_STUB_CLIENT_H
