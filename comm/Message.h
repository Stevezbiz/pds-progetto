//
// Created by Paolo Gastaldi on 18/11/2020.
//

#ifndef SERVER_MESSAGE_H
#define SERVER_MESSAGE_H

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <utility>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/bind/bind.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include "Comm_error.h"
#include "ElementStatus.h"

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

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version);

    Message(MESSAGE_TYPE code, std::string username, std::string password, std::string path, std::string hash, std::vector<std::string> paths, std::map<std::string, std::string> hashes, std::vector<unsigned char> file, ElementStatus elementStatus, Comm_error *comm_error, bool status);

    friend class boost::serialization::access;

public:
    MESSAGE_TYPE code;
    std::string username, password;
    std::string path, hash;
    std::vector<std::string> paths;
    std::map<std::string, std::string> hashes;
    std::vector<unsigned char> file;
    ElementStatus elementStatus;
    Comm_error *comm_error;
    bool status; // = okay

    struct_header_buffer *header_buffer;
    char *content_buffer;

    /**
    * send this message
    * using a TLV format (type, length, value)
    * @return a list of asio buffers
    */
    [[nodiscard]] std::vector<boost::asio::const_buffer> send() const;

    /**
     * check if the inner status is okay
     * @return boolean
     */
    [[nodiscard]] bool is_okay() const;

    /**
     * class constructor
     * @param code
     */
    explicit Message(MESSAGE_TYPE code = ERROR);

    /**
     * create a message to say everything is okay
     * @return new message
     */
    static Message *okay();

    /**
     * create a message to say something has gone wrong
     * @param comm_error
     * @return new message
     */
    static Message *error(Comm_error *comm_error = new Comm_error{});

    /**
     * create a message to perform login
     * @param username
     * @param password
     * @return new message
     */
    static Message *login(const std::string &username = "", const std::string &password = "");

    /**
     * create a message to send a probe command
     * a probe command gets a list of versions for all the given paths
     * it retrives a map <path, version>
     * @param paths
     * @return new message
     */
    static Message *probe(const std::vector <std::string> &paths);

    /**
     * create a message for a probe response
     * @param hashes - <path, version>
     * @return new message
     */
    static Message *probe_content(const std::map <std::string, std::string> &hashes);

    /**
     * create a message to retrieve a file with a specific path
     * @param path
     * @return new message
     */
    static Message *get(const std::string &path = "");

    /**
     * create a message for a get response
     * @param file
     * @param path
     * @return new message
     */
    static Message *get_content(const std::vector<unsigned char> &file, const std::string &path);

    /**
     * create a message to upload a file on the server
     * @param file
     * @param path
     * @param hash
     * @return new message
     */
    static Message *push(const std::vector<unsigned char> &file, const std::string &path = "", const std::string &hash = "", ElementStatus elementStatus = ElementStatus::modifiedFile);

    /**
     * create a message for a restore request
     * il returns the list of paths you have to ask to (with the get(...) method)
     * @return new message
     */
    static Message *restore();

    /**
     * create a message for a restore response
     * @param paths
     * @return new message
     */
    static Message *restore_content(const std::vector <std::string> &paths);

    /**
     * create a message for a end request
     * @return new message
     */
    static Message *end();

    /**
     * fill message header fields
     * @return modified message or new message
     */
    Message *build_header();

    /**
     * fill message content fields
     * @return modified message or new message
     */
    [[nodiscard]] Message *build_content() const;

    /**
     * get the header buffer
     * the header is made up by "type" and "length" of the message
     * @return header buffer
     */
    [[nodiscard]] boost::asio::mutable_buffer get_header_buffer() const;

    /**
     * get the content buffer
     * @return content buffer
     */
    [[nodiscard]] boost::asio::mutable_buffer get_content_buffer() const;

    ~Message();
};

#endif //SERVER_MESSAGE_H
