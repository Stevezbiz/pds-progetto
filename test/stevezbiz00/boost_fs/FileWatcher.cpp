//
// Created by stevezbiz on 01/11/2020.
//

#include "FileWatcher.h"
#include <thread>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

FileWatcher::FileWatcher(std::string path_to_watch, std::chrono::duration<int, std::milli> delay) :
        path_to_watch(std::move(path_to_watch)), delay(delay), running(true) {
    for (const auto &el : fs::recursive_directory_iterator(this->path_to_watch)) {
        if (fs::is_directory(el.path())) {
            FSElement dir{el.path(), ElementType::dir, fs::last_write_time(el.path())};
            this->paths.insert(std::make_pair(el.path().string(), dir));
        } else {
            FSElement file{el.path(), ElementType::file, fs::last_write_time(el.path())};
            this->paths.insert(std::make_pair(el.path().string(), file));
        }

    }
}

void FileWatcher::start(const std::function<void(std::string, ElementStatus)> &action) {
    while (this->running) {
        std::this_thread::sleep_for(this->delay);
        // controlla se un elemento è stato cancellato
        findErased(action);
        // cntrolla se un elemento è stato creato o modificato
        findCreatedOrModified(action);
    }
}

bool FileWatcher::contains(const std::string &key) {
    auto el = this->paths.find(key);
    return el != this->paths.end();
}

void FileWatcher::findErased(const std::function<void(std::string, ElementStatus)> &action) {
    auto it = this->paths.begin();
    while (it != this->paths.end()) {
        if (!fs::exists(it->first)) {
            if (it->second.getType() == ElementType::dir) {
                action(it->first, ElementStatus::erasedDir);
            } else {
                action(it->first, ElementStatus::erasedFile);
            }
            it = this->paths.erase(it);
        } else {
            it++;
        }
    }
}

void FileWatcher::findCreatedOrModified(const std::function<void(std::string, ElementStatus)> &action) {
    for (const auto &el : fs::recursive_directory_iterator(this->path_to_watch)) {
        time_t lwt = fs::last_write_time(el);
        if (!contains(el.path().string())) {
            if (fs::is_directory(el)) {
                this->paths.insert(std::make_pair(el.path().string(), FSElement{el.path(), ElementType::dir, lwt}));
                action(el.path().string(), ElementStatus::createdDir);
            } else {
                this->paths.insert(std::make_pair(el.path().string(), FSElement{el.path(), ElementType::file, lwt}));
                action(el.path().string(), ElementStatus::createdFile);
            }
        } else if (fs::is_regular_file(el)) {
            if (this->paths.find(el.path().string())->second.isOld(lwt)) {
                if (this->paths.find(el.path().string())->second.needUpdate()) {
                    action(el.path().string(), ElementStatus::modifiedFile);
                }
            }
        }
    }
}
