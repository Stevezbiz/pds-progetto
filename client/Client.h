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
    /**
     * Initializes the object's structures
     * @param root_path: the path that will be monitored
     * @param ip: ip address of the server
     * @param port: port to contact the server
     */
    Client(const std::string &root_path, std::string ip, std::string port);

    /**
     * Performs the login
     * @param username
     * @param password
     * @return true if the login has been successful, false otherwise
     */
    bool login(const std::string &username, const std::string &password);

    /**
     * Performs the probe
     * @return true if the probe has been successful, false otherwise
     */
    bool probe();

    /**
     * Performs the push
     * @param path
     * @param hash
     * @param status: the type of action to be pushed
     * @param fw_cycle: the iteration when the change has been recognized
     * @return true if the push has been successful, false otherwise
     */
    bool push(const std::string &path, const std::string &hash, ElementStatus status, int fw_cycle);

    /**
     * Performs the restore
     * @return true if the restore has been successful, false otherwise
     */
    bool restore();

    /**
     * Terminates the session and stops the monitoring for changes
     * @return true if the close has been successful, false otherwise
     */
    bool close();

    /**
     * Begins the monitoring for changes
     */
    void run();

    /**
     * Performs the login
     * @return true if the login has been successful, false otherwise
     */
    bool pwdAttempts();
};


#endif //CLIENT_CLIENT_H
