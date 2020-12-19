//
// Created by stevezbiz on 01/11/2020.
//

#ifndef BOOST_FS_FILEWATCHER_H
#define BOOST_FS_FILEWATCHER_H

#include <string>
#include <chrono>
#include <map>
#include <set>
#include <functional>
#include <thread>
#include <boost/filesystem.hpp>
#include <unordered_set>
#include "FSElement.h"
#include "../comm/ElementStatus.h"

class FileWatcher {
    std::string path_to_watch_;
    std::chrono::duration<int, std::milli> delay_;
    std::map<std::string, FSElement> files_;
    std::set<std::string> dirs_;
    std::atomic_bool running_;
    int fw_cycle_ = -1;

    /**
     * Verifies if the files' map contains a certain key
     * @param key
     * @return true if the files' map contains the key
     */
    bool contains(const std::string &key);

    /**
     * Checks for any element that has been removed since the previous check
     * @param action: handler funtion to manage the modifications
     */
    void find_erased(const std::function<void(std::string, std::string hash, ElementStatus, int fw_cycle)> &action);

    /**
     * Checks for any element that has been created or modified since the previous check
     * @param action: handler funtion to manage the modifications
     */
    void find_created_or_modified(
            const std::function<void(std::string, std::string hash, ElementStatus, int fw_cycle)> &action);

public:
    /**
     * Object constructor that initializes all the structures of the FileWatcher
     * @param path_to_watch: la directory che il file watcher deve monitorare
     * @param delay: il tempo di polling tra un check e il successivo
     */
    FileWatcher(std::string path_to_watch, std::chrono::duration<int, std::milli> delay);

    FileWatcher(const FileWatcher &) = delete;

    FileWatcher &operator=(const FileWatcher &) = delete;

    /**
     * Starts the FileWatcher, that will be able to recognize and signal modifications to the home directory content
     * @param action: handler funtion to manage the modifications
     */
    void start(const std::function<void(std::string, std::string hash, ElementStatus, int fw_cycle)> &action);

    /**
     * Clears all the collected data and get the initial image of the home directory
     */
    void init();

    /**
     * Getter for the files' map and the dirs' set
     * @return a map containing all the files and the directories associated with their hashes. Directories have an empty string as hash
     */
    std::map<std::string, std::string> get_elements();

    /**
     * Makes a path relative to the home directory of the FileWatcher
     * @param path
     * @return
     */
    [[nodiscard]] std::string parse_path(const boost::filesystem::path &path) const;

    /**
     * Stops the execution of the FileWatcher
     */
    void stop();
};


#endif //BOOST_FS_FILEWATCHER_H
