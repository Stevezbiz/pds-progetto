//
// Created by stevezbiz on 07/11/20.
//

#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include <boost/asio.hpp>
#include "../comm/Message.h"
#include "../comm/Client_API.h"
#include "FileWatcher.h"
#define MAX_ATTEMPTS 5

constexpr int FW_DELAY = 1000 * 5;

class Client {
    FileWatcher fw_;
    Client_API api_;
    std::string root_path_;
    std::future<void> f_;

public:
    Client(const std::string &root_path, std::string ip, std::string port);

    bool login(const std::string &username, const std::string &password);

    bool probe();

    bool push(const std::string &path, const std::string &hash, ElementStatus status);

    bool restore();

    bool close();

    void run();

    bool pwdAttempts();
};


#endif //CLIENT_CLIENT_H
