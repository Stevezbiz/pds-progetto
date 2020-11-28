//
// Created by Paolo Gastaldi on 28/11/2020.
//

#ifndef SERVER_STUB_CLIENT_H
#define SERVER_STUB_CLIENT_H

#include <iostream>
#include <utility>
#include "FileWatcher.h"
#include "../comm/Client_API.h"
#include "../comm/Client_socket_API.h"

constexpr int FS_DELAY = 1000*5; // 5 seconds

class Stub_client {
    FileWatcher *fw;
    Client_API *api;
    bool is_logged_in = false;
    std::map<std::string, std::string> map;

    /**
     * manage errors
     * @param error
     */
    static void error_handler_(Comm_error *error) {
        std::string s{ "[" + std::to_string(error->comm_errno) + "] " + error->message +  " (" + error->location + ")"};
        std::cout << s << std::endl;
    }

    /**
     * manager filewatcher change events
     * @param path
     * @param hash
     * @param es
     */
    void fw_handler_(const std::string &path, const std::string &hash, ElementStatus es) {
        auto file = Utils::read_from_file(path);
        if(!api->do_push(file, path, hash)) {
            this->error_handler_(this->api->get_last_error());
        }
    }

    /**
     * fill stub values
     */
    void prepare_stub() {
        this->map.insert(std::pair<std::string, std::string>{ "a.txt", "0x00" });
        this->map.insert(std::pair<std::string, std::string>{ "b.txt", "0x01" });
    }

public:
    Stub_client(const std::string &root_path, const std::string &ip, const std::string &port) {
        boost::asio::io_context ctx;
        boost::asio::ip::tcp::resolver resolver(ctx);
        auto endpoint_iterator = resolver.resolve({ ip, port });
        boost::asio::ip::tcp::socket socket{ ctx };
        boost::asio::connect(socket, endpoint_iterator);

        auto socket_api = new Client_socket_API{ std::move(socket) };
        this->api = new Client_API{socket_api};

        this->prepare_stub();
    }

    /**
     * perform login actions
     */
    bool login(const std::string &username, const std::string &password) {
        if(!this->is_logged_in) {
            this->is_logged_in = this->api->do_login(username, password);
            if(!this->is_logged_in)
                this->error_handler_(this->api->get_last_error());
        }

        return this->is_logged_in;
    }

    /**
     * launch client in normal (default) mode
     */
    bool normal() {
        if(!this->is_logged_in) return false;

        this->api->do_probe(this->map);
        this->fw->start(this->fw_handler_);

        return true;
    }

    /**
     * launch client in restore mode
     */
    bool restore() {
        if(!this->is_logged_in) return false;

        this->api->do_restore();
        this->normal();

        return true;
    }

    ~Stub_client() {
        delete this->api;
        delete this->fw;
    }
};


#endif //SERVER_STUB_CLIENT_H
