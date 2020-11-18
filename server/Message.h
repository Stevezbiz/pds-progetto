//
// Created by Paolo Gastaldi on 18/11/2020.
//

#ifndef SERVER_MESSAGE_H
#define SERVER_MESSAGE_H

#include <iostream>
#include <vector>
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
    RESTORE = 7
};

/**
 * create and send messages
 * to manage the transfer protocol
 * @author Gastaldi Paolo
 * @version 1.0.0
 */
class Message {
    MESSAGE_TYPE code;
    Message(MESSAGE_TYPE code = ERROR) : code(code), status(true) {}

    friend class boost::serialization::access;

public:
    std::string username, password;
    std::string path, hash;
    std::vector<std::string> paths;
    std::vector<std::string> hashes;
    std::vector<unsigned char> file;
    bool status; // = okay

    /**
     * sent this message
     * @param out_stream
     * @return stream
     */
    std::ostream send_to(std::ostream out_stream) {
        boost::archive::text_oarchive oa{ out_stream };

        oa << this->code;
        switch(this->code) {
            case OKAY:
                oa << this->status;
                break;
            case LOGIN:
                oa << this->username;
                oa << this->password;
                break;
            case PROBE:
                oa << this->paths;
                break;
            case PROBE_CONTENT:
                oa << this->hashes;
                break;
            case GET:
                oa << this->path;
                break;
            case GET_CONTENT:
                oa << this->file;
            case PUSH:
                oa << this->path;
                oa << this->file;
                oa << this->hash;
                break;
            default:
                oa << false;
        }
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
    static Message *okay() {
        auto message = new Message{ OKAY };
        return message;
    }

    /**
     * create a message to perform login
     * @param username
     * @param password
     * @return new message
     */
    static Message *login(std::string username = "", std::string password = "") {
        auto message = new Message{ LOGIN };
        message->username = username;
        message->password = password;
        return message;
    }

    /**
     * create a message to send a probe command
     * a probe command gets a list of versions for all the given paths
     * @param paths
     * @return new message
     */
    static Message *probe(std::vector<std::string> paths) {
        auto message = new Message{ PROBE };
        message->paths = paths;
        return message;
    }

    /**
     * create a message to retrieve a file with a specific path
     * @param path
     * @return new message
     */
    static Message *get(std::string path = "") {
        auto message = new Message{ GET };
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
    static Message *push(std::iostream file, std::string path = "", std::string hash = "") {
        auto message = new Message{ PUSH };
        message->file = std::vector<unsigned char>{ std::istreambuf_iterator<char>(file), {} };
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
};


#endif //SERVER_MESSAGE_H
