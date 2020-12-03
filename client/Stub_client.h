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
    static void error_handler_(Comm_error *error) {
        std::cout << error->to_string() << std::endl;
    }

    /**
     * fill stub values
     */
    void prepare_stub() {
        std::vector<std::string> paths{ "a.txt", "b.txt" }; // see these files in cmake-build-debug

        for(const auto &path : paths)
            this->map.insert(std::pair<std::string, std::string>{ path, Utils::SHA256(path) });
    }

    void open_conn_(const std::string &ip, const std::string &port) {
        Logger::info("Stub_client::open_conn_", "Opening connection to " + ip + ":" + port + "...", PR_LOW);
        try {
            boost::asio::io_context ctx;
            boost::asio::ip::tcp::resolver resolver(ctx);
            auto endpoint_iterator = resolver.resolve({ ip, port });
            boost::asio::ip::tcp::socket socket{ ctx };
            boost::asio::connect(socket, endpoint_iterator);

            auto socket_api = new Client_socket_API{ std::move(socket) };
            this->api = new Client_API{ socket_api };
        } catch(const std::exception &e) {
            std::cerr << "(Stub_client::open_conn_) " << e.what() << std::endl;
            exit(-1);
        }
        Logger::info("Stub_client::open_conn_", "Opening connection - done", PR_LOW);
    }

public:
    /**
     * class constructor
     * @param root_path
     * @param ip
     * @param port
     */
    Stub_client(const std::string &root_path, const std::string &ip, const std::string &port) {
        this->open_conn_(ip, port);
        this->fw = new FileWatcher{ root_path, std::chrono::milliseconds(FW_DELAY) };
        this->prepare_stub();
    }

    /**
     * perform login actions
     */
    bool login(const std::string &username, const std::string &password) {
        if(!this->is_logged_in) {
            Logger::info("Stub_client::login", "Login request...", PR_LOW);
            this->is_logged_in = this->api->login(username, password);
            if(!this->is_logged_in)
                Stub_client::error_handler_(this->api->get_message()->comm_error);
            Logger::info("Stub_client::login", "Login request - done", PR_LOW);
        }

        return this->is_logged_in;
    }

    /**
     * launch client in normal (default) mode
     */
    bool normal() {
        if(!this->is_logged_in) return false;

        Logger::info("Stub_client::normal", "Probe request...", PR_LOW);
        this->api->probe(this->map);
        Logger::info("Stub_client::normal", "Probe request - done", PR_LOW);
        Logger::info("Stub_client::normal", "Starting filewatcher...", PR_LOW);
        this->fw->start([this](const std::string &path, const std::string &hash, ElementStatus es) {
            auto file = Utils::read_from_file(path);
            if(!this->api->push(file, path, hash, es))
                Stub_client::error_handler_(this->api->get_last_error());
        });

        return true;
    }

    /**
     * launch client in restore mode
     */
    bool restore() {
        if(!this->is_logged_in) return false;

        Logger::info("Stub_client::normal", "Restore request...", PR_LOW);
        this->api->restore();
        Logger::info("Stub_client::normal", "Restore request - done", PR_LOW);
        this->normal();

        return true;
    }

    ~Stub_client() {
        delete this->api;
        delete this->fw;
    }
};


#endif //SERVER_STUB_CLIENT_H
