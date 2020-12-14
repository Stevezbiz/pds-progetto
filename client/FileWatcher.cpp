//
// Created by stevezbiz on 01/11/2020.
//

#include "FileWatcher.h"

namespace fs = boost::filesystem;

FileWatcher::FileWatcher(std::string path_to_watch, std::chrono::duration<int, std::milli> delay) :
        path_to_watch_(std::move(path_to_watch)), delay_(delay), running(true) {
//    path_offset_ = path_to_watch_.size();
//    if (path_to_watch_[path_offset_ - 1] != boost::filesystem::path::preferred_separator) {
//        path_to_watch_ += boost::filesystem::path::preferred_separator;
//        path_offset_++;
//    }
    init();
}

void FileWatcher::start(const std::function<void(std::string, std::string, ElementStatus)> &action) {
    while (running) {
        std::this_thread::sleep_for(delay_);
        findErased(action);
        findCreatedOrModified(action);
    }
}

bool FileWatcher::contains(const std::string &key) {
    auto el = files_.find(key);
    return el != files_.end();
}

void FileWatcher::findErased(const std::function<void(std::string, std::string, ElementStatus)> &action) {
    auto it = files_.begin();
    while (it != files_.end()) {
        if (!fs::exists(it->first)) {
            action(parse_path(it->first), "", ElementStatus::erasedFile);
            it = files_.erase(it);
        } else {
            it++;
        }
    }
    auto it2 = dirs_.end();
    while (it2 != dirs_.begin()) {
        if (!fs::exists(it2->data())) {
            action(parse_path(it2->data()), "", ElementStatus::erasedDir);
            it2 = dirs_.erase(it2);
        } else {
            it2--;
        }
    }
}

void FileWatcher::findCreatedOrModified(const std::function<void(std::string, std::string, ElementStatus)> &action) {
    for (const auto &el : fs::recursive_directory_iterator(path_to_watch_)) {
        if (fs::is_regular_file(el.path())) {
            time_t lwt = fs::last_write_time(el);
            if (!contains(el.path().string())) {
                FSElement new_file{el.path(), lwt};
                files_.insert(std::make_pair(el.path().string(), new_file));
                action(parse_path(el.path().string()), new_file.getHash(), ElementStatus::createdFile);
            } else {
                if (files_.find(el.path().string())->second.isOld(lwt)) {
                    if (files_.find(el.path().string())->second.needUpdate()) {
                        action(parse_path(el.path().string()), files_.find(el.path().string())->second.getHash(),
                               ElementStatus::modifiedFile);
                    }
                }
            }
        } else if (fs::is_directory(el.path())) {
            if (dirs_.find(el.path().string()) == dirs_.end()) {
                dirs_.insert(el.path().string());
                action(parse_path(el.path().string()), "", ElementStatus::createdDir);
            }
        }
    }
}

void FileWatcher::init() {
    files_.clear();
    dirs_.clear();
    for (const auto &el : fs::recursive_directory_iterator(path_to_watch_)) {
        if (fs::is_regular_file(el.path())) {
            FSElement file{el.path(), fs::last_write_time(el.path())};
            files_.insert(std::make_pair(el.path().string(), file));
        } else if (fs::is_directory(el.path())) {
            dirs_.insert(el.path().string());
        }
    }
}

std::map<std::string, std::string> FileWatcher::get_files() {
    std::map<std::string, std::string> map;
    for (auto it : files_) {
        map.insert(std::make_pair(parse_path(it.first), it.second.getHash()));
    }
    for (auto it:dirs_) {
        map.insert(std::make_pair(parse_path(it), ""));
    }
    return map;
}

std::string FileWatcher::parse_path(const boost::filesystem::path &path) const {
    return boost::filesystem::relative(path, path_to_watch_).string();
}
