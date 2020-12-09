//
// Created by stevezbiz on 26/11/20.
//

#include "Client_API.h"

//template class API<Client_socket_API>;

bool Client_API::get_and_save_(const std::string &path) {
    if(!this->api_->send_and_receive(Message::get(path), MSG_GET_CONTENT))
        return false;
    auto res = this->api_->get_message();
    if (!res->is_okay())
        return false;

    if (!Client_API::save_file_(res))
        return false;

    return true;
}

Client_API::Client_API(Client_socket_API *socket_api) : API(socket_api), api_(socket_api) {}

bool Client_API::login(const std::string &username, const std::string &password) {
    Logger::info("Client_API::login", "Trying to perform login...", PR_LOW);
    if(!this->api_->send_and_receive(Message::login(username, password), MSG_OKAY))
        return false;
    auto res = this->api_->get_message();
    Logger::info("Client_API::login", "Trying to perform login... - done", PR_LOW);

    return res->is_okay();
}

bool Client_API::probe(const std::map<std::string, std::string> &map) {
    Logger::info("Client_API::probe", "Probe check started...", PR_LOW);

    std::vector<std::string> paths;
    paths.reserve(map.size());
    for (auto const &item : map)
        paths.push_back(item.first);
    if(!this->api_->send_and_receive(Message::probe(paths), MSG_PROBE_CONTENT))
        return false;
    auto res = this->api_->get_message();

    if(!res->is_okay())
        return false;

    // if the client has a different file than the server, the client starts a push procedure
    for(auto const &item : res->hashes) {
        auto path = item.first;
        auto it = map.find(path);
        if(it == map.end()) { // if the file does not exists, the file has been created
            Logger::info("Client_API::probe", "Created file found" + path, PR_LOW);
            if (!this->push(Utils::read_from_file(path), path, map.at(path), ElementStatus::createdFile))
                return false;
        }
        else if(map.at(path) != item.second) { // if versions (hashes) are different, the file has been modified
            Logger::info("Client_API::probe", "Modified file found " + path, PR_LOW);
            if (!this->push(Utils::read_from_file(path), path, map.at(path),ElementStatus::modifiedFile))
                return false;
        }
    }

    for(auto const &item : map) {
        auto path = item.first;
        auto it = map.find(path);
        if(it == map.end()) { // if the file does not exists, the file has been deleted
            Logger::info("Client_API::probe", "Deleted file found " + path, PR_LOW);
            if (!this->push(Utils::read_from_file(path), path, "0x0", ElementStatus::erasedFile))
                return false;
        }
    }
    Logger::info("Client_API::probe", "Probe check started... - done", PR_LOW);

    return true;
}

bool Client_API::push(const std::vector<unsigned char> &file, const std::string &path, const std::string &hash, ElementStatus elementStatus) {
    Logger::info("Client_API::push", "Push started...");
    if(!this->api_->send_and_receive(Message::push(file, path, hash, elementStatus), MSG_OKAY))
        return false;
    auto res = this->api_->get_message();
    Logger::info("Client_API::push", "Push started... - done");

    return res->is_okay();
}

bool Client_API::restore() {
    Logger::info("Client_API::restore", "Restore started...", PR_LOW);
    if(!this->api_->send_and_receive(Message::restore(), MSG_RESTORE_CONTENT))
        return false;
    auto res = this->api_->get_message();

    if(!res->is_okay())
        return false;

    for(const auto &path : res->paths) {
        if (!this->get_and_save_(path))
            return false;
    }
    Logger::info("Client_API::restore", "Restore started... - done", PR_LOW);

    return true;
}

bool Client_API::end() {
    Logger::info("Client_API::end", "End started...", PR_LOW);
    if(!this->api_->send_and_receive(Message::end(), MSG_OKAY))
        return false;
    auto res = this->api_->get_message();
    Logger::info("Client_API::end", "End started... - done", PR_LOW);

    return res->is_okay();
}
