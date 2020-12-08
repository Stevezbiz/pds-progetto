//
// Created by stevezbiz on 07/11/20.
//

#include "Session.h"

Session::Session(int session_id) : session_id(session_id) {}

std::string Session::get_cookie() const {
    return Utils::sign_cookie(std::to_string(this->session_id)); // TODO: make this more secure
}

bool Session::is_logged_in() const {
    return this->login_status;
}
