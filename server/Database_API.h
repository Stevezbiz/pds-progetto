//
// Created by stevezbiz on 05/12/20.
//

#ifndef TEST_SOCKET_DATABASE_API_H
#define TEST_SOCKET_DATABASE_API_H

#include <sqlite3.h>
#include <string>
#include <unordered_map>

class Database_API {
    sqlite3 *db_ = nullptr;

public:
    /**
     *
     * @param path
     */
    explicit Database_API(std::string path);

    /**
     *
     * @param username
     * @param password
     * @return
     */
    [[nodiscard]] bool login_query(const std::string &username, const std::string &password) const;

    std::unordered_map<std::string,std::string> get_paths(const std::string &username);

    bool save_paths(const std::unordered_map<std::string,std::string> &map, const std::string &username);

    ~Database_API();
};


#endif //TEST_SOCKET_DATABASE_API_H
