//
// Created by stevezbiz on 07/11/20.
//

#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H

#include <iostream>
#include "../comm/Utils.h"

class Session {
    
public:
    int session_id{ -1 };
    std::string user;
    bool login_status{ false };

    /**
     * class constructor
     * @param session_id
     */
    explicit Session(int session_id);

    /**
     * genereate a cookie to identify this session
     * @return cookie
     */
    std::string get_cookie() const;

    /**
     * check if login has been performed
     * @return login status
     */
    bool is_logged_in() const;

};

#endif //SERVER_SESSION_H
