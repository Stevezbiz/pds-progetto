//
// Created by stevezbiz on 05/11/20.
//

#ifndef SERVER_MESSAGE_H
#define SERVER_MESSAGE_H

#include <string>

#define HEADER_LENGTH 4
#define MAX_BODY_LENGTH 1024


class Message {
    char data_[HEADER_LENGTH + MAX_BODY_LENGTH + 1]{};
    std::size_t body_length_;
public:
    enum {
        header_length = HEADER_LENGTH, max_body_length = MAX_BODY_LENGTH
    };

    Message();

    char *getData();

    std::size_t getLength() const;

    char *getBody();

    std::size_t getBodyLength() const;

    void setBody(char *body);

    bool decodeHeader();

    void encodeHeader();
};


#endif //SERVER_MESSAGE_H
