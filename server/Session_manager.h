//
// Created by Paolo Gastaldi on 07/12/2020.
//

#ifndef TEST_SOCKET_SESSION_MANAGER_H
#define TEST_SOCKET_SESSION_MANAGER_H

#include <iostream>
#include <map>
#include "../comm/Message.h"
#include "./Session.h"
#include "../comm/Utils.h"

class Session_manager {
    std::atomic<int> session_counter_{0};
    std::map<int, Session *> sessions_{};

public:
    Session_manager() = default;

    /**
     * get the session given a message
     * @param message
     * @return session
     */
    Session *retrieve_session(const Message *message);
};


#endif //TEST_SOCKET_SESSION_MANAGER_H
