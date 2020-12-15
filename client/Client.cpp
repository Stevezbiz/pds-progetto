//
// Created by stevezbiz on 07/11/20.
//

#include "Client.h"

#include <utility>

Client::Client(const std::string &root_path, std::string ip, std::string port)
        : root_path_(root_path), api_(new Client_socket_API{std::move(ip), std::move(port)}, root_path),
          fw_(root_path, std::chrono::duration<int, std::milli>(FW_DELAY)) {}

bool Client::login(const std::string &username, const std::string &password) {
    return api_.login(username, password);
}

bool Client::probe() {
    fw_.init();
    return api_.probe(fw_.get_files());
}

bool Client::push(const std::string &path, const std::string &hash, ElementStatus status, int fw_cycle) {
    Logger::info("Client::push", "push started for file " + path, PR_VERY_LOW);
    std::vector<unsigned char> file;
    boost::filesystem::path dest_path{root_path_};
    dest_path.append(path);
    if (status == ElementStatus::createdFile || status == ElementStatus::modifiedFile) {
        file = Utils::read_from_file(dest_path);
    }
    return api_.push(file, path, hash, status, fw_cycle);
}

bool Client::restore() {
    bool ret = api_.restore();
    fw_.init();
    return ret;
}

bool Client::close() {
    auto ret_val = api_.end();
    fw_.running.store(false);
    f_.get();
    return ret_val;
}

void Client::run() {
    f_ = std::async(std::launch::async, [this](){
        this->fw_.start([this](const std::string  &path, const std::string &hash, ElementStatus status, int fw_cycle) {
            if (!push(path, hash, status, fw_cycle)) {
                this->close();
            }
        });
    });

}

bool Client::pwdAttempts() {
    std::cout << "*************** LOGIN ***************" << std::endl;
    std::string username;
    std::string password;
    for (int tries = 0; tries < MAX_ATTEMPTS; ++tries)
    {
        std::cout << "Please insert username: ";
        std::cin >> username;
        std::cout << "Please insert password: ";
        std::cin >> password;
        if (Client::login(username, Utils::hash(password))) {
            std::cout << "\n| Welcome " << username << " |" <<std::endl;
            return true;
        }
        std::cout << "\nPlease try again.\n";
    }
    return false;
}

