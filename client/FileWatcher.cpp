//
// Created by stevezbiz on 01/11/2020.
//

#include "FileWatcher.h"
#include "../comm/Logger.h"

namespace fs = boost::filesystem;

FileWatcher::FileWatcher(std::string path_to_watch, std::chrono::duration<int, std::milli> delay) :
        path_to_watch_(std::move(path_to_watch)), delay_(delay), running_(true) {
    init();
}

bool FileWatcher::start(const std::function<bool(std::string, std::string, ElementStatus, int)> &action) {
    Logger::info("FileWatcher::start", "Started monitoring", PR_NORMAL);
    while (running_) {
        if(!find_erased(action))
            return false;
        if(!find_created_or_modified(action))
            return false;
        fw_cycle_++;
        std::this_thread::sleep_for(delay_);
    }
    Logger::info("FileWatcher::start", "Stopped monitoring", PR_NORMAL);
    return true;
}

bool FileWatcher::contains(const std::string &key) {
    auto el = files_.find(key);
    return el != files_.end();
}

bool FileWatcher::find_erased(const std::function<bool(std::string, std::string, ElementStatus, int)> &action) {
    auto it = files_.begin();
    while (it != files_.end()) {
        if (!fs::exists(it->first)) {
            Logger::info("FileWatcher::find_erased", "Found erased file " + it->first, PR_VERY_LOW);
            if(!action(parse_path(it->first), it->second.getHash(), ElementStatus::erasedFile, fw_cycle_))
                return false;
            it = files_.erase(it);
        } else {
            it++;
        }
    }
    auto it2 = dirs_.rbegin();
    while (it2 != dirs_.rend()) {
        if (!fs::exists(*it2)) {
            Logger::info("FileWatcher::find_erased", "Found erased directory " + *it2, PR_VERY_LOW);
            if(!action(parse_path(*it2), "", ElementStatus::erasedDir, fw_cycle_))
                return false;
            auto it3 = dirs_.erase(--it2.base());
            it2 = std::reverse_iterator(it3);
        } else {
            it2++;
        }
    }
    return true;
}

bool FileWatcher::find_created_or_modified(
        const std::function<bool(std::string, std::string, ElementStatus, int)> &action) {
    for (const auto &el : fs::recursive_directory_iterator(path_to_watch_)) {
        if (fs::is_regular_file(el.path())) {
            time_t lwt = fs::last_write_time(el);
            if (!contains(el.path().string())) {
                FSElement new_file{el.path(), lwt};
                files_.insert(std::make_pair(el.path().string(), new_file));
                Logger::info("FileWatcher::find_created_or_modified", "Found created file " + el.path().string(),
                             PR_VERY_LOW);
                if(!action(parse_path(el.path().string()), new_file.getHash(), ElementStatus::createdFile, fw_cycle_))
                    return false;
            } else {
                if (files_.find(el.path().string())->second.isOld(lwt)) {
                    if (files_.find(el.path().string())->second.needUpdate()) {
                        Logger::info("FileWatcher::find_created_or_modified",
                                     "Found modified file " + el.path().string(), PR_VERY_LOW);
                        if(!action(parse_path(el.path().string()), files_.find(el.path().string())->second.getHash(),
                               ElementStatus::modifiedFile, fw_cycle_))
                            return false;
                    }
                }
            }
        } else if (fs::is_directory(el.path())) {
            if (dirs_.find(el.path().string()) == dirs_.end()) {
                dirs_.insert(el.path().string());
                Logger::info("FileWatcher::find_created_or_modified", "Found created directory " + el.path().string(),
                             PR_VERY_LOW);
                if(!action(parse_path(el.path().string()), "", ElementStatus::createdDir, fw_cycle_))
                    return false;
            }
        }
    }
    return true;
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
    fw_cycle_ = 0;
    Logger::info("FileWatcher::init", "Initialization done", PR_LOW);
}

std::map<std::string, std::string> FileWatcher::get_elements() {
    std::map<std::string, std::string> map;
    for (auto it : files_) {
        map.insert(std::make_pair(parse_path(it.first), it.second.getHash()));
    }
    for (auto it:dirs_) {
        map.insert(std::make_pair(parse_path(it), ""));
    }
    return map;
}

std::string FileWatcher::parse_path(const fs::path &path) const {
    return fs::relative(path, path_to_watch_).string();
}

void FileWatcher::stop() {
    running_ = false;
}
