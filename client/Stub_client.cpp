//
// Created by Paolo Gastaldi on 28/11/2020.
//

#include "Stub_client.h"

void Stub_client::error_handler_(Comm_error *error = new Comm_error{}) {
    std::cout << "Error handling: " + error->to_string() << std::endl;
}

void Stub_client::prepare_stub() {
    std::vector<std::string> paths{ "./client_files/a.txt", "./client_files/b.txt" }; // see these files in cmake-build-debug

    for(const auto &path : paths)
        this->map.insert(std::pair<std::string, std::string>{ path, Utils::SHA256(path) });
}

Stub_client::Stub_client(const std::string &root_path, const std::string &ip, const std::string &port) {
    this->api = new Client_API{ new Client_socket_API{ ip, port }};
    this->fw = new FileWatcher{ root_path, std::chrono::milliseconds(FW_DELAY) };
    this->prepare_stub();
}

bool Stub_client::login(const std::string &username, const std::string &password) {
    if(!this->is_logged_in) {
        Logger::info("Stub_client::login", "Login request...", PR_LOW);
        this->is_logged_in = this->api->login(username, password);
        if(!this->is_logged_in) {
            Logger::info("Stub_client::login", "Login request failed", PR_LOW);
            Stub_client::error_handler_(this->api->get_last_error());
        }
        Logger::info("Stub_client::login", "Login request - done", PR_LOW);
    }

    return this->is_logged_in;
}

bool Stub_client::normal() {
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

bool Stub_client::restore() {
    if(!this->is_logged_in) return false;

    Logger::info("Stub_client::normal", "Restore request...", PR_LOW);
    this->api->restore();
    Logger::info("Stub_client::normal", "Restore request - done", PR_LOW);
    this->normal();

    return true;
}

Stub_client::~Stub_client() {
    delete this->api;
    delete this->fw;
}
