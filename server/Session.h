//
// Created by stevezbiz on 07/11/20.
//

#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H

#include <iostream>
#include "../comm/Utils.h"

class Session {
    int session_id_{ -1 };
    bool login_status_{ false };

public:
    std::string user;

    /**
     * class constructor
     * @param session_id
     */
    explicit Session(int session_id);

    /**
     * genereate a cookie to identify this session
     * @return cookie
     */
    std::string get_cookie();

    /**
     * check if login has been performed
     * @return login status
     */
    bool is_logged_in();

};

#endif //SERVER_SESSION_H
