//
// Created by stevezbiz on 07/11/20.
//

#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H

#include <iostream>
#include <unordered_map>
#include "../comm/Utils.h"

class Session {
    std::unordered_map<std::string, std::string> files_;

    /**
     *
     * @param key
     * @return
     * @deprecated
     */
    bool files_contains(const std::string &key);

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

    /**
     *
     * @param path
     * @param hash
     * @return
     * @deprecated
     */
    bool create_file(const std::string &path, const std::string &hash);

    /**
     *
     * @param path
     * @param hash
     * @return
     * @deprecated
     */
    bool modify_file(const std::string &path, const std::string &hash);

    /**
     *
     * @param path
     * @return
     * @deprecated
     */
    bool remove_file(const std::string &path);

    /**
     *
     * @return
     * @deprecated
     */
    const std::vector<std::string> *get_paths();

    /**
     *
     * @deprecated
     */
    const std::unordered_map<std::string, std::string> *get_files();
};

#endif //SERVER_SESSION_H
