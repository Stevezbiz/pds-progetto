//
// Created by stevezbiz on 26/11/20.
//

#include "Server_API.h"

#include <utility>

std::shared_ptr<Message> Server_API::do_login_(const std::shared_ptr<Session>& session, const std::shared_ptr<Message>& req) {
    auto status = this->login_(session.get(), req->username, req->password);
    session->login_status = status;
    return status ? Message::okay() : Message::error(new Comm_error{CE_WRONG_VALUE, "Server_API::do_login", "Invalid username or password" });
}

std::shared_ptr<Message> Server_API::do_probe_(const std::shared_ptr<Session>& session, const std::shared_ptr<Message>& req) {
    auto hashes = this->probe_(session.get(), req->paths);
    return Message::probe_content(*hashes);
}

std::shared_ptr<Message> Server_API::do_get_(const std::shared_ptr<Session>& session, const std::shared_ptr<Message>& req) {
    ElementStatus status;
    auto file = this->get_(session.get(), req->path, status);
    return Message::get_content(*file, req->path, status);
}

std::shared_ptr<Message> Server_API::do_push_(const std::shared_ptr<Session>& session, const std::shared_ptr<Message>& req) {
    auto status = this->push_(session.get(), req->path, req->file, req->hash, req->elementStatus);
    return status ? Message::okay() : Message::error(new Comm_error{CE_FAILURE, "Server_API::do_push", "Unable to push the file" });
}

std::shared_ptr<Message> Server_API::do_restore_(const std::shared_ptr<Session>& session, const std::shared_ptr<Message>& req) {
    auto paths = this->restore_(session.get());
    return Message::restore_content(*paths);
}

std::shared_ptr<Message> Server_API::do_end_(const std::shared_ptr<Session>& session, const std::shared_ptr<Message>& req) {
    auto status = this->end_(session.get());
    return status ? Message::okay() : Message::error(new Comm_error{CE_FAILURE, "Server_API::do_end", "The server doesn't approved your request" });
}

void Server_API::do_handle_error_(const std::shared_ptr<Session>& session, const std::shared_ptr<Comm_error>& comm_error) {
    this->handle_error_(session.get(), comm_error.get());
}

bool Server_API::terminate_(std::shared_ptr<Socket_API> api) {
    Logger::info("Server_API::terminate_", "Terminating...", PR_LOW);
    auto status = api->close_conn(true); // generate error in Socket_API::call_, if the timeout is over
    api.reset();
    Logger::info("Server_API::terminate_", "Running... - done", PR_LOW);
    return status;
}

Server_API::Server_API(std::shared_ptr<Session_manager> session_manager) : session_manager_(std::move(session_manager)) {}

void Server_API::set_login(const std::function<bool(Session *, const std::string &, const std::string &)> &login_function) {
    this->login_ = login_function;
}

void Server_API::set_probe(const std::function<const std::unordered_map<std::string, std::string> *(Session *, const std::vector<std::string> &)> &probe_function) {
    this->probe_ = probe_function;
}

void Server_API::set_get(const std::function<const std::vector<unsigned char> *(Session *, const std::string &, ElementStatus &)> &get_function) {
    this->get_ = get_function;
}

void Server_API::set_push(const std::function<bool(Session *, const std::string &, const std::vector<unsigned char> &, const std::string &, ElementStatus)> &push_function) {
    this->push_ = push_function;
}

void Server_API::set_restore(const std::function<const std::vector<std::string> *(Session *)> &restore_function) {
    this->restore_ = restore_function;
}

void Server_API::set_end(const std::function<bool(Session *)> &end_function) {
    this->end_ = end_function;
}

void Server_API::set_handle_error(const std::function<void(Session *, const Comm_error *)> &handler_error_function) {
    this->handle_error_ = handler_error_function;
}

bool Server_API::run(std::unique_ptr<Socket_API> api, int socket_timeout) {
    bool end_session = false;
    bool keep_alive;
    bool status = true;
    std::future<bool> f;
    std::shared_ptr<Socket_API> api_ = std::move(api); // unique_ptr to shared_ptr, as it is used in more threads

    Logger::info("Server_API::run", "Running...", PR_LOW);
    do {
        f = std::async(std::launch::async, [](const std::shared_ptr<Socket_API> &api_) { return api_->receive(MSG_UNDEFINED); }, api_);
        auto future_status = f.wait_for(std::chrono::milliseconds(socket_timeout));
        if(future_status == std::future_status::timeout) {
            Logger::warning("Server_API::run", "Receive timeout");
            status = api_->shutdown();
            f.wait();
//            status = api->close_conn(true); // generate error in Socket_API::call_, if the timeout is over
            break;
        }
        if(!f.get()) {
            Logger::info("Server_API::run", "Receive error", PR_VERY_LOW);
            this->do_handle_error_(this->session_manager_->get_empty_session(), api_->get_last_error());
            api_->send(Message::error(new Comm_error{CE_GENERIC, "Server_API::run", "Transmission level error"}));
            break;
        }

        auto req = api_->get_message();
        keep_alive = req->keep_alive;
        Logger::info("Server_API::run", "Keep alive: " + std::to_string(keep_alive), PR_VERY_LOW);
        auto session = this->session_manager_->retrieve_session(req);
        if (req->code != MSG_LOGIN && !session->is_logged_in()) {
            api_->send(Message::error(new Comm_error{CE_NOT_ALLOWED, "Server_API::run", "Login must be perfomed before this action"}));
            break;
        }

        std::shared_ptr<Message> res;

        // manage the request and produce a response message
        switch (req->code) {
            case MSG_OKAY:
                res = Message::okay();
                break;
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
                end_session = true;
                break;
            default:
                res = Message::error(new Comm_error{CE_UNEXPECTED_TYPE, "Server_API::run", "Invalid message code"});
        }

        res->cookie = session->get_cookie();
        api_->send(res);
        res.reset();
        if(end_session)
            this->session_manager_->remove_session(session);
    } while(keep_alive);

//    Logger::info("Server_API::discard", "Terminating...", PR_LOW);
//    status = api_->close_conn(true); // generate error in Socket_API::call_, if the timeout is over
//    api_.reset();
//    Logger::info("Server_API::discard", "Running... - done", PR_LOW);
    session_manager_->pause_session(api_->get_message());

    return this->terminate_(api_) && status;
}

bool Server_API::discard(std::unique_ptr<Socket_API> api) {
    std::future<bool> f;
    std::shared_ptr<Socket_API> api_ = std::move(api); // unique_ptr to shared_ptr, as it is used in more threads

    // no receive
    auto res = Message::retry_later();
    res->cookie = COOKIE_KEEP_THE_SAME;
    api_->send(res);

    return this->terminate_(api_);
}