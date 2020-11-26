//
// Created by Paolo Gastaldi on 18/11/2020.
//

#ifndef SERVER_MESSAGE_H
#define SERVER_MESSAGE_H

#include <iostream>
#include <sstream>
#include <vector>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

/**
 * list of possible messages supported by the protocol
 * @author Gastaldi Paolo
 * @version 1.0.0
 */
enum MESSAGE_TYPE : int {
    ERROR = -1,
    UNDEFINED = 0,
    OKAY = 1,
    LOGIN = 2,
    PROBE = 3,
    PROBE_CONTENT = 4,
    GET = 5,
    GET_CONTENT = 6,
    PUSH = 7,
    RESTORE = 8,
    RESTORE_CONTENT = 9,
    END = 100
};

/**
 * create and send messages
 * to manage the transfer protocol
 * @author Gastaldi Paolo
 * @version 1.0.0
 */
class Message {
    enum lengths {
        type_length = sizeof(typeof(MESSAGE_TYPE)),
        length_length = sizeof(size_t)
    };

    struct struct_header_buffer {
        MESSAGE_TYPE type;
        size_t length;
    };

    friend class boost::serialization::access;

public:
    MESSAGE_TYPE code;
    std::string username, password;
    std::string path, hash;
    std::vector<std::string> paths;
    std::map<std::string, std::string> hashes;
    std::vector<unsigned char> file;
    bool status; // = okay

    struct_header_buffer *header_buffer;
    char *content_buffer;

    std::vector <boost::asio::const_buffer> send();

    [[nodiscard]] bool is_okay() const;

    explicit Message(MESSAGE_TYPE code = ERROR);

    static Message *okay();

    static Message *error();

    static Message *login(const std::string &username = "", const std::string &password = "");

    static Message *probe(const std::vector <std::string> &paths);

    static Message *probe_content(const std::map <std::string, std::string> &hashes);

    static Message *get(const std::string &path = "");

    static Message *get_content(const std::vector<unsigned char> &file, const std::string &path);

    static Message *
    push(const std::vector<unsigned char> &file, const std::string &path = "", const std::string &hash = "");

    static Message *restore();

    static Message *restore_content(const std::vector <std::string> &paths);

    static Message *end();

    Message *build();

    [[nodiscard]] boost::asio::const_buffer get_header_buffer() const;

    [[nodiscard]] boost::asio::const_buffer get_content_buffer() const;
};

#endif //SERVER_MESSAGE_H
