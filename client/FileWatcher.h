//
// Created by stevezbiz on 01/11/2020.
//

#ifndef BOOST_FS_FILEWATCHER_H
#define BOOST_FS_FILEWATCHER_H

#include <string>
#include <chrono>
#include <unordered_map>
#include <functional>
#include "FSElement.h"

enum class ElementStatus {
    createdFile, modifiedFile, erasedFile
};

class FileWatcher {
    std::string path_to_watch_;
    std::chrono::duration<int, std::milli> delay_;
    std::unordered_map<std::string, FSElement> paths_;
    bool running_;

    bool contains(const std::string &key);

    void findErased(const std::function<void(std::string, ElementStatus)> &action);

    void findCreatedOrModified(const std::function<void(std::string, ElementStatus)> &action);

public:

    FileWatcher(std::string path_to_watch, std::chrono::duration<int, std::milli> delay);

    FileWatcher(const FileWatcher &) = delete;

    FileWatcher &operator=(const FileWatcher &) = delete;

    void start(const std::function<void(std::string, ElementStatus)> &action);
};


#endif //BOOST_FS_FILEWATCHER_H
