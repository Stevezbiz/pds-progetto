//
// Created by stevezbiz on 07/11/20.
//

#include "Session.h"

Session::Session(int session_id) : session_id_(session_id) {}

std::string Session::get_cookie() {
    return Utils::sign_cookie(std::to_string(this->session_id_)); // TODO: make this more secure
}

bool Session::is_logged_in() {
    return this->login_status_;
}
