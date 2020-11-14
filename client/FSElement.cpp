//
// Created by stevezbiz on 02/11/20.
//

#include "FSElement.h"
#include <iostream>
#include <utility>
#include <iomanip>
#include <openssl/sha.h>

namespace fs = boost::filesystem;

FSElement::FSElement(fs::path path, ElementType type, time_t last_write_time) : path_(std::move(path)), type_(type),
                                                                                last_write_time_(last_write_time) {
    SHA256();
}

void FSElement::SHA256() {
    if (type_ == ElementType::dir) {
        std::size_t buf_size{path_.leaf().string().length()};
        char buf[buf_size];
        unsigned char digest[SHA256_DIGEST_LENGTH] = {0};

        SHA256_CTX ctx;
        SHA256_Init(&ctx);

        memcpy(buf, path_.leaf().string().data(), buf_size);
        SHA256_Update(&ctx, buf, buf_size);

        SHA256_Final(digest, &ctx);

        std::ostringstream os;
        os << std::hex << std::setfill('0');
        for (unsigned char i : digest) {
            os << std::setw(2) << i;
        }
        hash_ = os.str();
    } else {
        std::ifstream fp(path_.string(), std::ios::in | std::ios::binary);
        if (!fp.good()) {
            std::ostringstream os;
            os << "Cannot open " << path_ << " :" << std::strerror(errno);
            throw std::runtime_error(os.str());
        }
        std::size_t buf_size{4096};
        char buf[buf_size];
        unsigned char digest[SHA256_DIGEST_LENGTH] = {0};

        SHA256_CTX ctx;
        SHA256_Init(&ctx);

        while (fp.good()) {
            fp.read(buf, buf_size);
            SHA256_Update(&ctx, buf, fp.gcount());
        }
        fp.close();

        SHA256_Final(digest, &ctx);

        std::ostringstream os;
        os << std::hex << std::setfill('0');
        for (unsigned char i : digest) {
            os << std::setw(2) << i;
        }
        hash_ = os.str();
    }
}

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
    SHA256();
    if (old != hash_) {
        return true;
    } else {
        return false;
    }
}

ElementType FSElement::getType() {
    return type_;
}
