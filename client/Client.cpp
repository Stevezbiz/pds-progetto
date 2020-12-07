//
// Created by stevezbiz on 07/11/20.
//

#include "Client.h"

#include <utility>

Client::Client(const std::string &root_path, boost::asio::ip::tcp::socket socket)
        : root_path_(root_path), api_(new Socket_API{std::move(socket)}),
          fw_(root_path, std::chrono::duration<int, std::milli>(FW_DELAY)) {}

bool Client::login(const std::string &username, const std::string &password) {
    return api_.login(username, password);
}

bool Client::probe() {
    fw_.init();
    return api_.probe(fw_.get_files());
}

bool Client::push(const std::string &path, const std::string &hash, ElementStatus status) {
    std::vector<unsigned char> file;
    boost::filesystem::path dest_path{root_path_};
    dest_path.append(path);
    if (status != ElementStatus::erasedFile) {
        file = Utils::read_from_file(dest_path);
    }
    return api_.push(file, path, hash, status);
}

bool Client::restore() {
    bool ret = api_.restore();
    fw_.init();
    return ret;
}

bool Client::close() {
    return api_.end();
}

void Client::run() {
    fw_.start([this](std::string path, std::string hash, ElementStatus status) {
        if(!push(path, hash, status)){
            // TODO: error management
        }
    });
}
