//
// Created by stevezbiz on 02/11/20.
//

#include "FSElement.h"

namespace fs = boost::filesystem;

FSElement::FSElement(const fs::path &path, time_t last_write_time) : path_(path), last_write_time_(last_write_time),
                                                                     hash_(Utils::SHA256(path)) {}

bool FSElement::isOld(time_t lwt) {
    if (last_write_time_ == lwt) {
        return false;
    } else {
        last_write_time_ = lwt;
        return true;
    }
}

bool FSElement::needUpdate() {
    std::string old = hash_;
    hash_ = Utils::SHA256(path_);
    if (old != hash_) {
        return true;
    } else {
        return false;
    }
}

std::string FSElement::getHash() {
    return hash_;
}
