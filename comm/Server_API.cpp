//
// Created by stevezbiz on 26/11/20.
//

#include "Server_API.h"

Message *Server_API::do_login(Message *req) {
    auto status = Server_API::login(req->username, req->password);
    return status ? Message::okay() : Message::error(new Comm_error{ WRONG_VALUE, "Server_API::do_login", "Invalid username or password" });
}

Message *Server_API::do_probe(Message *req) {
    auto hashes = Server_API::probe(req->paths);
    return Message::probe_content(*hashes);
}

Message *Server_API::do_get(Message *req) {
    auto file = Server_API::get(req->path);
    return Message::get_content(*file, req->path);
}

Message *Server_API::do_push(Message *req) {
    auto status = Server_API::push(req->path, req->file, req->hash, req->elementStatus);
    return status ? Message::okay() : Message::error(new Comm_error{ FAILURE, "Server_API::do_push", "Unable to push the file" });
}

Message *Server_API::do_restore(Message *req) {
    auto paths = Server_API::restore();
    return Message::restore_content(*paths);
}

Message *Server_API::do_end(Message *req) {
    auto status = Server_API::end();
    return status ? Message::okay() : Message::error(new Comm_error{ FAILURE, "Server_API::do_end", "The server doesn't approved your request" });
}

Server_API::Server_API(Socket_API *socket_api, const std::string &user_root_path) : API(socket_api, user_root_path) {}

void Server_API::set_login(const std::function<bool(const std::string &, const std::string &)> &login_function) {
    login = login_function;
}

void Server_API::set_probe(const std::function<const std::map<std::string, std::string> *(const std::vector<std::string> &)> &probe_function) {
    Server_API::probe = probe_function;
}

void Server_API::set_get(const std::function<const std::vector<unsigned char> *(const std::string &)> &get_function) {
    Server_API::get = get_function;
}

void Server_API::set_push(const std::function<bool(const std::string &, const std::vector<unsigned char> &, const std::string &, ElementStatus)> &push_function) {
    Server_API::push = push_function;
}

void Server_API::set_restore(const std::function<const std::vector<std::string> *()> &restore_function) {
    Server_API::restore = restore_function;
}

void Server_API::set_end(const std::function<bool()> &end_function) {
    Server_API::end = end_function;
}

void Server_API::run() {
    bool stop = false;

    while(!stop) {
        this->api->receive(UNDEFINED);
        auto req = this->api->get_message();
        Message *res;

        // manage the request and produce a response message
        switch (req->code) {
            case LOGIN:
                res = Server_API::do_login(req);
                break;
            case PROBE:
                res = Server_API::do_probe(req);
                break;
            case GET:
                res = Server_API::do_get(req);
                break;
            case PUSH:
                res = Server_API::do_push(req);
                break;
            case RESTORE:
                res = Server_API::do_restore(req);
                break;
            case END:
                res = Server_API::do_end(req);
                stop = true;
                break;
            default:
                res = Message::error(new Comm_error{UNEXPECTED_TYPE, "Server_API::run", "Message code not valid"});
        }

        this->api->send(res);
    }
}

// default functions
std::function<bool(const std::string &, const std::string &)> Server_API::login = [](const std::string &, const std::string &) {
    return true;
};

std::function<const std::map<std::string, std::string> *(const std::vector <std::string> &)> Server_API::probe = [](const std::vector <std::string> &) {
    return new std::map<std::string, std::string>{};
};

std::function<const std::vector<unsigned char> *(const std::string &)> Server_API::get = [](const std::string &) {
    return new std::vector<unsigned char>{};
};

std::function<bool(const std::string &, const std::vector<unsigned char> &, const std::string &, ElementStatus)> Server_API::push = [](const std::string &, const std::vector<unsigned char> &, const std::string &, ElementStatus) {
    return true;
};

std::function<const std::vector<std::string> *()> Server_API::restore = []() {
    return new std::vector<std::string>{};
};

std::function<bool()> Server_API::end = []() {
    return true;
};