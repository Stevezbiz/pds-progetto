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

    [[nodiscard]] std::unordered_map<std::string,std::string> get_path_schema(const std::string &username) const;

    [[nodiscard]] bool save_path_schema(const std::unordered_map<std::string,std::string> &map, const std::string &username) const;

    [[nodiscard]] bool insert_path(const std::string &path, const std::string &hash, const std::string &username) const;

    [[nodiscard]] bool delete_path(const std::string &path) const;

    [[nodiscard]] bool update_path(const std::string &path, const std::string &hash) const;

    ~Database_API();
};


#endif //TEST_SOCKET_DATABASE_API_H
