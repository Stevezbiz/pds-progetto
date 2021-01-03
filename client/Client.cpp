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
    return api_.probe(fw_.get_elements());
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
    if (ret) {
        fw_.init();
        Logger::info("Client::restore", "Restore completed", PR_NORMAL);
    }
    return ret;
}

bool Client::close() {
    Logger::info("Client::close", "Closing the socket", PR_VERY_LOW);
    auto ret_val = api_.end();
    Logger::info("Client::close", "Closing...", PR_NORMAL);
    fw_.stop();
    if(f_.valid())
        f_.get();
    Logger::info("Client::close", "Closing... - done", PR_VERY_LOW);
    return ret_val;
}

void Client::run() {
    f_ = std::async(std::launch::async, [this]() {
        if (!this->fw_.start([this](const std::string &path, const std::string &hash, ElementStatus status, int fw_cycle) {
                    if (!push(path, hash, status, fw_cycle)) {
                        Logger::error("Client::run", "Cannot push changes of '" + path + "' on server");
                        std::cout << "Cannot push changes of '" + path + "' on server" << std::endl;
                        return false;
                    }
                    return true;
                })) {
            std::cout << "Closing..." << std::endl;
            auto ret_val = api_.end();
            fw_.stop();
            exit(-1);
        }
    });

}

bool Client::pwdAttempts() {
    std::cout << "*************** LOGIN ***************" << std::endl;
    std::string username;
    std::string password;
    for (int tries = 0; tries < MAX_ATTEMPTS; ++tries) {
        std::cout << "Please insert username: ";
        std::cin >> username;
        std::cout << "Please insert password: ";
        std::cin >> password;
        if (Client::login(username, password)) {
            std::cout << "\n| Welcome " << username << " |" << std::endl;
            return true;
        }
        std::cout << "\nPlease try again.\n";
    }
    auto ret_val = api_.end();
    return false;
}

