//
// Created by stevezbiz on 01/11/2020.
//

#include "FileWatcher.h"

namespace fs = boost::filesystem;

FileWatcher::FileWatcher(std::string path_to_watch, std::chrono::duration<int, std::milli> delay) :
        path_to_watch_(std::move(path_to_watch)), delay_(delay), running_(true) {
    path_offset_ = path_to_watch_.size();
    if (path_to_watch_[path_offset_ - 1] != '/') {
        path_to_watch_ += '/';
        path_offset_++;
    }
    init();
}

void FileWatcher::start(const std::function<void(std::string, std::string, ElementStatus)> &action) {
    while (running_) {
        std::this_thread::sleep_for(delay_);
        findErased(action);
        findCreatedOrModified(action);
    }
}

bool FileWatcher::contains(const std::string &key) {
    auto el = paths_.find(key);
    return el != paths_.end();
}

void FileWatcher::findErased(const std::function<void(std::string, std::string, ElementStatus)> &action) {
    auto it = paths_.begin();
    while (it != paths_.end()) {
        if (!fs::exists(it->first)) {
            action(parse_path(it->first), "", ElementStatus::erasedFile);
            it = paths_.erase(it);
        } else {
            it++;
        }
    }
}

void FileWatcher::findCreatedOrModified(const std::function<void(std::string, std::string, ElementStatus)> &action) {
    for (const auto &el : fs::recursive_directory_iterator(path_to_watch_)) {
        if (fs::is_regular_file(el.path())) {
            time_t lwt = fs::last_write_time(el);
            if (!contains(el.path().string())) {
                FSElement new_file{el.path(), lwt};
                paths_.insert(std::make_pair(el.path().string(), new_file));
                action(parse_path(el.path().string()), new_file.getHash(), ElementStatus::createdFile);
            } else {
                if (paths_.find(el.path().string())->second.isOld(lwt)) {
                    if (paths_.find(el.path().string())->second.needUpdate()) {
                        action(parse_path(el.path().string()), paths_.find(el.path().string())->second.getHash(),
                               ElementStatus::modifiedFile);
                    }
                }
            }
        }
    }
}

void FileWatcher::init() {
    paths_.clear();
    for (const auto &el : fs::recursive_directory_iterator(path_to_watch_)) {
        if (fs::is_regular_file(el.path())) {
            FSElement file{el.path(), fs::last_write_time(el.path())};
            paths_.insert(std::make_pair(el.path().string(), file));
        }
    }
}

std::unordered_map<std::string, std::string> FileWatcher::get_files() {
    std::unordered_map<std::string, std::string> map;
    for (auto it : paths_) {
        map.insert(std::make_pair(it.first, it.second.getHash()));
    }
    return map;
}

std::string FileWatcher::parse_path(const boost::filesystem::path &path) const {
    return path.string().substr(path_offset_, path.string().size());
}
