//
// Created by stevezbiz on 26/11/20.
//

#include "Client_API.h"

bool Client_API::get_and_save_(const std::string &path) {
    auto req = Message::get(path);
    this->api.async_send(req);
    if(!this->api.receive(GET_CONTENT))
        return false;
    auto res = this->api.get_message();
    if (!res->is_okay())
        return false;

    if (!this->save_file_(res))
        return false;

    return true;
}

Client_API::Client_API(Socket_API &socket_api, const std::string &root_path) : API(socket_api, root_path) {}

bool Client_API::do_login(const std::string &username, const std::string &password) {
    auto req = Message::login(username, password);
    this->api.async_send(req);
    if(!this->api.receive(OKAY))
        return false;
    auto res = this->api.get_message();

    return res->is_okay();
}

bool Client_API::do_probe(const std::map<std::string, std::string> &map) {
    std::vector<std::string> paths;
    paths.reserve(map.size());
    for (auto const &item : map)
        paths.push_back(item.first);
    auto req = Message::probe(paths);
    this->api.async_send(req);
    if(!this->api.receive(PROBE_CONTENT))
        return false;
    auto res = this->api.get_message();

    if(!res->is_okay())
        return false;

    for(auto const &item : res->hashes) {
        auto path = item.first;
        auto it = map.find(path);
        if(it == map.end() || map.at(path) != item.second) { // check if the file not exists or if versions (hashes) are different
            // if the client has a different file than the server, the client starts a push procedure
            if(!this->do_push(Utils::read_from_file(path), path, map.at(path))) {
                return false;
            }
        }
    }

    return true;
}

bool Client_API::do_push(const std::vector<unsigned char> &file, const std::string &path, const std::string &hash) {
    auto req = Message::push(file, path, hash);
    this->api.async_send(req);
    if(!this->api.receive(OKAY))
        return false;
    auto res = this->api.get_message();

    return res->is_okay();
}

bool Client_API::do_restore() {
    auto req = Message::restore();
    this->api.async_send(req);
    if(!this->api.receive(RESTORE_CONTENT))
        return false;
    auto res = this->api.get_message();

    if(!res->is_okay())
        return false;

    for(const auto &path : res->paths)
        if (!this->get_and_save_(path))
            return false;

    return true;
}

bool Client_API::do_end() {
    auto req = Message::end();
    this->api.async_send(req);
    if(!this->api.receive(OKAY))
        return false;
    auto res = this->api.get_message();

    return res->is_okay();
}
