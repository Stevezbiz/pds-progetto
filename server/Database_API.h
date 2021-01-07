//
// Created by stevezbiz on 05/12/20.
//

#ifndef TEST_SOCKET_DATABASE_API_H
#define TEST_SOCKET_DATABASE_API_H

#include <sqlite3.h>
#include <string>
#include <unordered_map>

enum PathType {
    file = 0, dir = 1
};

class Database_API {
    sqlite3 *db_{nullptr};

public:
    /**
     * Creates the API to access the database
     * @param path: the path of the database to be opened
     */
    explicit Database_API(std::string path);

    /**
     * Executes the login query to the database
     * @param username: username value to search
     * @param password: password value to search
     * @return true if the query finds a correspondance, false otherwise or in case of an error occurred
     */
    [[nodiscard]] bool login_query(const std::string &username, const std::string &password) const;

    /**
     * Executes a query to find all the paths stored for a specified user
     * @param username: the user to look for
     * @return a vector containing all the paths stored in the database for the selected user
     */
    [[nodiscard]] std::vector<std::string> *get_path_schema(const std::string &username) const;

    /**
     * Inserts a record in the database
     * @param path
     * @param hash
     * @param username
     * @param type
     * @return true if the record has been successfully inserted, false otherwise
     */
    [[nodiscard]] bool insert_path(const std::string &path, const std::string &hash, const std::string &username,
                                   PathType type) const;

    /**
     * Deletes a record from the database
     * @param path
     * @return true if the record has been successfully deleted, false otherwise
     */
    [[nodiscard]] bool delete_path(const std::string &path) const;

    /**
     * Updates a record of the database
     * @param path
     * @param hash
     * @return true if the record has been successfully updated, false otherwise
     */
    [[nodiscard]] bool update_path(const std::string &path, const std::string &hash) const;

    /**
     * Executes a query to find all the paths and hashes stored for a specified user
     * @param username
     * @return a map containing paths and hashes
     */
    [[nodiscard]] std::unordered_map<std::string, std::string> *get_schema(const std::string &username) const;

    ~Database_API();
};


#endif //TEST_SOCKET_DATABASE_API_H
