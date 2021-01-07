//
// Created by stevezbiz on 07/11/20.
//

#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H

#include <boost/asio.hpp>
#include <iostream>
#include <unordered_map>
#include "../comm/Socket_API.h"
#include "Database_API.h"

constexpr int MAX_THREADS_PER_SESSION = 2;

class Session {

public:
    int session_id{-1};
    std::string user;
    bool login_status{false};
    std::atomic<int> thread_count{0};
    std::mutex m_;
    std::atomic_bool active_thread_{false};
    std::condition_variable cv_;

    /**
     * Initializesthe object's structures
     * @param session_id
     */
    explicit Session(int session_id);

    /**
     * Genereates a cookie to identify this session
     * @return cookie
     */
    [[nodiscard]] std::string get_cookie() const;

    /**
     * Checks if login has been performed
     * @return login status
     */
    [[nodiscard]] bool is_logged_in() const;

    /**
     * Inserts a file in the database
     * @param database: the database
     * @param path: the path of the file
     * @param hash: the hash of the file
     * @return the result of the action on the database
     */
    [[nodiscard]] bool create_file(const Database_API &database, const std::string &path, const std::string &hash) const;

    /**
     * Updates a file already in the database
     * @param database: the database
     * @param path: the path of the file
     * @param hash: the hash of the file
     * @return the result of the action on the database
     */
    static bool modify_file(const Database_API &database, const std::string &path, const std::string &hash);

    /**
     * Removes a file from the database
     * @param database: the database
     * @param path: the path of the file
     * @return the result of the action on the database
     */
    static bool remove_file(const Database_API &database, const std::string &path);

    /**
     * Retrieves the elements from the database
     * @param database: the database
     * @return a vector containing the paths of the retrieved elements
     */
    [[nodiscard]] const std::vector<std::string> *get_path_schema(const Database_API &database) const;

    /**
     * Retrieves the elements from the database
     * @param database: the database
     * @return a map containing the paths and the hashes of the retrieved elements
     */
    [[nodiscard]] const std::unordered_map<std::string, std::string> *get_schema(const Database_API &database) const;

    /**
     * Insert a directory in the database
     * @param database: the database
     * @param path: the path of the directory
     * @param hash: an empty string
     * @return the result of the action on the database
     */
    [[nodiscard]] bool create_dir(const Database_API &database, const std::string &path, const std::string &hash) const;

    /**
     * Removes a directory from the database
     * @param database: the database
     * @param path: the path of the directory
     * @return the result of the action on the database
     */
    static bool remove_dir(const Database_API &database, const std::string &path);
};

#endif //SERVER_SESSION_H
