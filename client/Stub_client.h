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
    std::unique_ptr<FileWatcher> fw;
    std::unique_ptr<Client_API> api;
    bool is_logged_in = false;
    std::map<std::string, std::string> map; // stub map

    /**
     * manage errors
     * @param error
     */
    static void error_handler_(const std::shared_ptr<Comm_error> &error);

    /**
     * fill stub values
     */
    void prepare_stub();

    /**
     * open a connection
     * @param ip
     * @param port
     */
    void open_conn_(const std::string &ip, const std::string &port);

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
};


#endif //SERVER_STUB_CLIENT_H
