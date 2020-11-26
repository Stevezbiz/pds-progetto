//
// Created by Paolo Gastaldi on 18/11/2020.
//

#ifndef SERVER_MESSAGE_H
#define SERVER_MESSAGE_H

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "Comm_error.h"

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

    ~Message() {
        delete this->content_buffer;
    }

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

    /**
     * send this message
     * using a TLV format (type, length, value)
     * @return asio buffer
     */
    std::vector<boost::asio::const_buffer> send() {
        std::ostringstream type_stream{};
        std::ostringstream length_stream{};
        std::ostringstream value_stream{};
        boost::archive::text_oarchive oa{ value_stream };

        // serialize message content
        oa << this;
        auto value_data = value_stream.str();

        // fixed lengths for the "type" and "length" values
        type_stream << std::setw(type_length) << std::hex << this->code;
        length_stream << std::setw(length_length) << std::hex << value_data.length();
        auto type_data = value_stream.str();
        auto length_data = value_stream.str();

        // concat buffers into a vector
        std::vector<boost::asio::const_buffer> out_buffers;
        out_buffers.emplace_back(boost::asio::buffer(type_data));
        out_buffers.emplace_back(boost::asio::buffer(length_data));
        out_buffers.emplace_back(boost::asio::buffer(value_data));

        return out_buffers;
    }

    /**
     * check if the inner status is okay
     * @return boolean
     */
    [[nodiscard]] bool is_okay() const { return this->status; }

    explicit Message(MESSAGE_TYPE code = ERROR) : code(code), status(true), header_buffer(new struct_header_buffer{ ERROR, 0 }), content_buffer(nullptr) {
        if(code == ERROR)
            status = false;
    }

    /**
     * create a message to say everything is okay
     * @return new message
     */
    static Message *okay() {
        return new Message{ OKAY };
    }

    /**
     * create a message to say something has gone wrong
     * @return new message
     */
    static Message *error() {
        return new Message{ ERROR };
    }

    /**
     * create a message to perform login
     * @param username
     * @param password
     * @return new message
     */
    static Message *login(const std::string &username = "", const std::string &password = "") {
        auto message = new Message{ LOGIN };
        message->username = username;
        message->password = password;
        return message;
    }

    /**
     * create a message to send a probe command
     * a probe command gets a list of versions for all the given paths
     * it retrives a map <path, version>
     * @param paths
     * @return new message
     */
    static Message *probe(const std::vector<std::string> &paths) {
        auto message = new Message{ PROBE };
        message->paths = paths;
        return message;
    }

    /**
     * create a message for a probe response
     * @param hashes - <path, version>
     * @return new message
     */
    static Message *probe_content(const std::map<std::string, std::string> &hashes) {
        auto message = new Message{ PROBE_CONTENT };
        message->hashes = hashes;
        return message;
    }

    /**
     * create a message to retrieve a file with a specific path
     * @param path
     * @return new message
     */
    static Message *get(const std::string &path = "") {
        auto message = new Message{ GET };
        message->path = path;
        return message;
    }

    /**
     * create a message for a get response
     * @param file
     * @param path
     * @return new message
     */
    static Message *get_content(const std::vector<unsigned char> &file, const std::string &path) {
        auto message = new Message{ GET_CONTENT };
        message->file = file;
        message->path = path;
        return message;
    }

    /**
     * create a message to upload a file on the server
     * @param file
     * @param path
     * @param hash
     * @return new message
     *
     * DO NOT change the params order, no default value for the std::iostream
     */
    static Message *push(const std::vector<unsigned char> &file, const std::string &path = "", const std::string &hash = "") {
        auto message = new Message{ PUSH };
        message->file = file;
        message->path = path;
        message->hash = hash;
        return message;
    }

    /**
     * create a message for a restore request
     * il returns the list of paths you have to ask to (with the get(...) method)
     * @return new message
     */
    static Message *restore() {
        auto message = new Message{ RESTORE };
        return message;
    }

    /**
     * create a message for a restore response
     * @param paths
     * @return new message
     */
    static Message *restore_content(const std::vector<std::string> &paths) {
        auto message = new Message{ RESTORE_CONTENT };
        message->paths = paths;
        return message;
    }

    /**
     * create a message for a end request
     * @return new message
     */
    static Message *end() {
        return new Message{ END };
    }

    /**
     * fill message fields
     * @return modified message or new message
     */
    Message *build() {
        if(this->content_buffer == nullptr) { // if only the header has been received
            this->code = this->header_buffer->type;
            this->content_buffer = new char[this->header_buffer->length]; // prepare the content buffer
            return this;
        }

        // else if the content as been received
        std::stringstream ss{ this->content_buffer };
        boost::archive::text_iarchive ia{ ss };
        auto *new_message = new Message{};
        ia >> *new_message; // de-serialization
        return new_message;
    }

    /**
     * get the header buffer
     * the header is made up by "type" and "length" of the message
     * @return header buffer
     */
    [[nodiscard]] boost::asio::const_buffer get_header_buffer() const { // generic pointer, sorry
        return boost::asio::const_buffer(this->header_buffer, sizeof(struct_header_buffer));
    }

    /**
     * get the content buffer
     * @return content buffer
     */
    [[nodiscard]] boost::asio::const_buffer get_content_buffer() const { // generic pointer, sorry
        return boost::asio::const_buffer(this->content_buffer, sizeof(this->header_buffer->length));
    }
};


#endif //SERVER_MESSAGE_H
