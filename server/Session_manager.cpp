//
// Created by Paolo Gastaldi on 07/12/2020.
//

#include "Session_manager.h"

Session *Session_manager::retrieve_session(const Message *message) {
    auto plain_cookie = Utils::verify_cookie(message->cookie);
    auto it = this->sessions_.find(std::stoi(plain_cookie));
    if(it != this->sessions_.end()) // if already exists
        return it->second;

    int new_session_id = this->session_counter_++;
    auto new_session = new Session(new_session_id);
    this->sessions_.insert_or_assign(new_session_id, new_session);
    return new_session;
}