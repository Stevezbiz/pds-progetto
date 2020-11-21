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
    OKAY = 0,
    LOGIN = 1,
    PROBE = 2,
    PROBE_CONTENT = 3,
    GET = 4,
    GET_CONTENT = 5,
    PUSH = 6,
    RESTORE = 7,
    RESTORE_CONTENT = 8
};

/**
 * create and send messages
 * to manage the transfer protocol
 * @author Gastaldi Paolo
 * @version 1.0.0
 */
class Message {
    enum {
        type_length = sizeof(int),
        length_length = sizeof(size_t)
    };

    Message(MESSAGE_TYPE code = ERROR) : code(code), status(true) {}

    friend class boost::serialization::access;

public:
    MESSAGE_TYPE code;
    std::string username, password;
    std::string path, hash;
    std::vector<std::string> paths;
    std::vector<std::string> hashes;
    std::vector<unsigned char> file;
    bool status; // = okay

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
        out_buffers.push_back(boost::asio::buffer(type_data));
        out_buffers.push_back(boost::asio::buffer(length_data));
        out_buffers.push_back(boost::asio::buffer(value_data));

        return out_buffers;
    }

    /**
     * check if the inner status is okay
     * @return boolean
     */
    bool isOkay() { return this->status; }

    /**
     * build a message from the TCP stream received
     * @param in_stream
     */
    Message(std::istream in_stream) : Message() {
        std::istringstream archive_stream{};
        boost::archive::text_iarchive ia{ in_stream };

        MESSAGE_TYPE code;
        ia >> this->code;
        switch(code) {
            case OKAY:
                ia >> this->okay;
                break;
            case LOGIN:
                ia >> this->username;
                ia >> this->password;
                break;
            case PROBE:
                ia >> this->paths;
                break;
            case PROBE_CONTENT:
                ia >> this->hashes;
                break;
            case GET:
                ia >> this->path;
                break;
            case GET_CONTENT:
                ia >> this->file;
            case PUSH:
                ia >> this->path;
                ia >> this->file;
                ia >> this->hash;
                break;
            default:
                this->status = false;
        }
    }

    /**
     * create a message to say everything is okay
     * @return new message
     */
    static std::vector<boost::asio::const_buffer> okay() {
        Message message{ OKAY };
        return message.send();
    }

    /**
     * create a message to perform login
     * @param username
     * @param password
     * @return new message
     */
    static std::vector<boost::asio::const_buffer> login(std::string username = "", std::string password = "") {
        Message message{ LOGIN };
        message.username = username;
        message.password = password;
        return message.send();
    }

    /**
     * create a message to send a probe command
     * a probe command gets a list of versions for all the given paths
     * @param paths
     * @return new message
     */
    static std::vector<boost::asio::const_buffer> probe(std::vector<std::string> paths) {
        Message message{ PROBE };
        message.paths = paths;
        return message.send();
    }

    /**
     * create a message to retrieve a file with a specific path
     * @param path
     * @return new message
     */
    static std::vector<boost::asio::const_buffer> get(std::string path = "") {
        Message message{ GET };
        message.path = path;
        return message.send();
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
    static std::vector<boost::asio::const_buffer> push(std::iostream file, std::string path = "", std::string hash = "") {
        Message message{ PUSH };
        message.file = std::vector<unsigned char>{ std::istreambuf_iterator<char>(file), {} };
        message.path = path;
        message.hash = hash;
        return message.send();
    }

    /**
     * create a message for a restore request
     * il returns the list of paths you have to ask to (with the get(...) method)
     * @return new message
     */
    static std::vector<boost::asio::const_buffer> restore() {
        Message message{ RESTORE };
        return message.send();
    }
};


#endif //SERVER_MESSAGE_H
