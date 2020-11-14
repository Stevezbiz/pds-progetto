//
// Created by stevezbiz on 05/11/20.
//

#include "Message.h"
#include <utility>
#include <cstring>

Message::Message() : body_length_(0) {}

char *Message::getData() {
    data_[header_length + body_length_] = '\0';
    return data_;
}

std::size_t Message::getLength() const {
    return header_length + body_length_;
}

char *Message::getBody() {
    data_[header_length + body_length_] = '\0';
    return data_ + header_length;
}

std::size_t Message::getBodyLength() const {
    return body_length_;
}

void Message::setBody(char *body) {
    body_length_ = strlen(body);
    if (body_length_ > max_body_length) {
        body_length_ = max_body_length;
    }
    memcpy(data_ + header_length, body, body_length_);
    data_[header_length + body_length_] = '\0';
}

bool Message::decodeHeader() {
    char header[header_length + 1] = "";
    std::strncat(header, data_, header_length);
    body_length_ = std::stoi(header);
    if (body_length_ > max_body_length) {
        body_length_ = 0;
        return false;
    }
    return true;
}

void Message::encodeHeader() {
    char header[header_length + 1];
    std::sprintf(header, "%4d", static_cast<int>(body_length_));
    std::memcpy(data_, header, header_length);
}

