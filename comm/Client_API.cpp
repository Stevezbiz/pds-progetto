//
// Created by stevezbiz on 26/11/20.
//

#include "Client_API.h"

bool Client_API::get_and_save_(const std::string &path) {
    auto req = Message::get(path);
    this->api_->send(req);
    if(!this->api_->receive(MSG_GET_CONTENT))
        return false;
    auto res = this->api_->get_message();
    if (!res->is_okay())
        return false;

    if (!this->save_file_(res))
        return false;

    return true;
}

Client_API::Client_API(Socket_API *socket_api, const std::string &root_path) : API(socket_api, root_path) {}

bool Client_API::login(const std::string &username, const std::string &password) {
    auto req = Message::login(username, password);
    this->api_->send(req);
    if(!this->api_->receive(MSG_OKAY))
        return false;
    auto res = this->api_->get_message();

    return res->is_okay();
}

bool Client_API::probe(const std::map<std::string, std::string> &map) {
    std::vector<std::string> paths;
    paths.reserve(map.size());
    for (auto const &item : map)
        paths.push_back(item.first);
    auto req = Message::probe(paths);
    this->api_->send(req);
    if(!this->api_->receive(MSG_PROBE_CONTENT))
        return false;
    auto res = this->api_->get_message();

    if(!res->is_okay())
        return false;

    // if the client has a different file than the server, the client starts a push procedure
    for(auto const &item : res->hashes) {
        auto path = item.first;
        auto it = map.find(path);
        if(it == map.end()) { // if the file does not exists, the file has been created
            if (!this->push(Utils::read_from_file(this->root_path / path), path, map.at(path), ElementStatus::createdFile))
                return false;
        }
        else if(map.at(path) != item.second) // if versions (hashes) are different, the file has been modified
            if(!this->push(Utils::read_from_file(this->root_path / path), path, map.at(path), ElementStatus::modifiedFile))
                return false;
    }

    for(auto const &item : map) {
        auto path = item.first;
        auto it = map.find(path);
        if(it == map.end()) // if the file does not exists, the file has been deleted
            if (!this->push(Utils::read_from_file(this->root_path / path), path, "0x0", ElementStatus::erasedFile))
                return false;
    }

    return true;
}

bool Client_API::push(const std::vector<unsigned char> &file, const std::string &path, const std::string &hash, ElementStatus elementStatus) {
    auto req = Message::push(file, path, hash, elementStatus);
    this->api_->send(req);
    if(!this->api_->receive(MSG_OKAY))
        return false;
    auto res = this->api_->get_message();

    return res->is_okay();
}

bool Client_API::restore() {
    auto req = Message::restore();
    this->api_->send(req);
    if(!this->api_->receive(MSG_RESTORE_CONTENT))
        return false;
    auto res = this->api_->get_message();

    if(!res->is_okay())
        return false;

    for(const auto &path : res->paths)
        if (!this->get_and_save_(path))
            return false;

    return true;
}

bool Client_API::end() {
    auto req = Message::end();
    this->api_->send(req);
    if(!this->api_->receive(MSG_OKAY))
        return false;
    auto res = this->api_->get_message();

    return res->is_okay();
}
