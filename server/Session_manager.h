//
// Created by Paolo Gastaldi on 07/12/2020.
//

#ifndef TEST_SOCKET_SESSION_MANAGER_H
#define TEST_SOCKET_SESSION_MANAGER_H

#include <iostream>
#include <map>
#include <locale>
#include "../comm/Message.h"
#include "./Session.h"
#include "../comm/Utils.h"

class Session_manager {
    std::atomic_int session_counter_{1};
    std::map<int, std::shared_ptr<Session>> sessions_{};

public:
    Session_manager() = default;

    /**
     * get the session given a message
     * @param message
     * @return session
     */
    std::shared_ptr<Session> retrieve_session(const std::shared_ptr<Message> &message);

    /**
     * save inner session status before the next call
     * @param message
     */
    void pause_session(const std::shared_ptr<Message> &message);

    /**
     * remove a session from the list
     * @param session
     * @return status
     */
    bool remove_session(const std::shared_ptr<Session> &session);

    ~Session_manager();

    static std::shared_ptr<Session> get_empty_session();
};


#endif //TEST_SOCKET_SESSION_MANAGER_H
