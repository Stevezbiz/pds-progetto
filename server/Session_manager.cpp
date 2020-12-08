//
// Created by Paolo Gastaldi on 07/12/2020.
//

#include "Session_manager.h"

Session *Session_manager::retrieve_session(const Message *message) {
    auto plain_cookie = Utils::verify_cookie(message->cookie);
    std::locale loc;
    if(std::isdigit(plain_cookie[0], loc)) {
        Logger::info("Session_manager::retrieve_session", "cookie: " + plain_cookie, PR_VERY_LOW);
        auto it = this->sessions_.find(std::stoi(plain_cookie));
        if (it != this->sessions_.end()) // if already exists
            return it->second;
    }

    int new_session_id = this->session_counter_++;
    auto new_session = new Session(new_session_id);
    this->sessions_.insert_or_assign(new_session_id, new_session);
    return new_session;
}

bool Session_manager::remove_session(Session *session) {
    auto it = this->sessions_.find(session->session_id);
    if(it == this->sessions_.end())
        return false;

    delete it->second;
    this->sessions_.erase(session->session_id);
    return true;
}

Session_manager::~Session_manager() {
    for(const auto &item : this->sessions_) {
        delete item.second;
    }
}
