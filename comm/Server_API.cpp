//
// Created by stevezbiz on 26/11/20.
//

#include "Server_API.h"
#include "Comm_error.h"
#include "Message.h"

Message *Server_API::do_login_(Session &session, Message *req) {
    auto status = this->login_(session, req->username, req->password);
    return status ? Message::okay() : Message::error(new Comm_error{CE_WRONG_VALUE, "Server_API::do_login", "Invalid username or password" });
}

Message *Server_API::do_probe_(Session &session, Message *req) {
    auto hashes = this->probe_(session, req->paths);
    return Message::probe_content(*hashes);
}

Message *Server_API::do_get_(Session &session, Message *req) {
    auto file = this->get_(session, req->path);
    return Message::get_content(*file, req->path);
}

Message *Server_API::do_push_(Session &session, Message *req) {
    auto status = this->push_(session, req->path, req->file, req->hash, req->elementStatus);
    return status ? Message::okay() : Message::error(new Comm_error{CE_FAILURE, "Server_API::do_push", "Unable to push the file" });
}

Message *Server_API::do_restore_(Session &session, Message *req) {
    auto paths = this->restore_(session);
    return Message::restore_content(*paths);
}

Message *Server_API::do_end_(Session &session, Message *req) {
    auto status = this->end_(session);
    return status ? Message::okay() : Message::error(new Comm_error{CE_FAILURE, "Server_API::do_end", "The server doesn't approved your request" });
}

void Server_API::do_handle_error_(Session &session, Comm_error *comm_error) {
    this->handle_error_(session, comm_error);
}

Server_API::Server_API(const std::string &user_root_path) {}

void Server_API::set_login(const std::function<bool(Session &, const std::string &, const std::string &)> &login_function) {
    this->login_ = login_function;
}

void Server_API::set_probe(const std::function<const std::map<std::string, std::string> *(Session &, const std::vector<std::string> &)> &probe_function) {
    this->probe_ = probe_function;
}

void Server_API::set_get(const std::function<const std::vector<unsigned char> *(Session &, const std::string &)> &get_function) {
    this->get_ = get_function;
}

void Server_API::set_push(const std::function<bool(Session &, const std::string &, const std::vector<unsigned char> &, const std::string &, ElementStatus)> &push_function) {
    this->push_ = push_function;
}

void Server_API::set_restore(const std::function<const std::vector<std::string> *(Session &)> &restore_function) {
    this->restore_ = restore_function;
}

void Server_API::set_end(const std::function<bool(Session &)> &end_function) {
    this->end_ = end_function;
}

void Server_API::set_handle_error(const std::function<void(Session &, const Comm_error *)> &handler_error_function) {
    this->handle_error_ = handler_error_function;
}

void Server_API::run(Session &session) {
    bool stop = false;

    while(!stop) {
        if(!session->api->receive(MSG_UNDEFINED)) {
            Server_API::do_handle_error_(Session session, session->api->get_last_error());
            session->api->send(Message::error(new Comm_error{CE_GENERIC, "Server_API::run", "Transmission level error" }));
            continue;
        }
        auto req = session->api->get_message();
        Message *res;

        // manage the request and produce a response message
        switch (req->code) {
            case MSG_LOGIN:
                res = this->do_login_(session, req);
                break;
            case MSG_PROBE:
                res = this->do_probe_(session, req);
                break;
            case MSG_GET:
                res = this->do_get_(session, req);
                break;
            case MSG_PUSH:
                res = this->do_push_(session, req);
                break;
            case MSG_RESTORE:
                res = this->do_restore_(session, req);
                break;
            case MSG_END:
                res = this->do_end_(session, req);
                stop = true;
                break;
            default:
                res = Message::error(new Comm_error{CE_UNEXPECTED_TYPE, "Server_API::run", "Message code not valid"});
        }

        session->api->send(res);
    }
}