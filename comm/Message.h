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
#include <boost/serialization/string.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <stdexcept>
#include "Comm_error.h"
#include "ElementStatus.h"
#include "Logger.h"

/**
 * list of possible messages supported by the protocol
 * @author Gastaldi Paolo
 * @version 1.0.0
 */
enum MESSAGE_TYPE : std::int32_t {
    MSG_ERROR = -1,
    MSG_UNDEFINED = 0,
    MSG_OKAY = 1,
    MSG_LOGIN = 2,
    MSG_PROBE = 3,
    MSG_PROBE_CONTENT = 4,
    MSG_GET = 5,
    MSG_GET_CONTENT = 6,
    MSG_PUSH = 7,
    MSG_RESTORE = 8,
    MSG_RESTORE_CONTENT = 9,
    MSG_END = 100
};

/**
 * create and send messages
 * to manage the transfer protocol
 * @author Gastaldi Paolo
 * @version 1.0.0
 */
class Message {
    enum lengths {
        code_length = sizeof(MESSAGE_TYPE),
        length_length = sizeof(std::size_t)
    };
//    struct struct_header_buffer {
//        MESSAGE_TYPE type;
//        std::size_t length;
//    };

    char *header_buffer_ = new char[code_length + length_length]();
    char *content_buffer_ = nullptr;
    std::size_t content_buffer_length_ = 0;
    std::string header_data_;
    std::string content_data_;
    std::string file_string_;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version);

    /**
     * class constructor
     * @param code
     * @param username
     * @param password
     * @param path
     * @param hash
     * @param paths
     * @param hashes
     * @param file
     * @param elementStatus
     * @param comm_error
     * @param status
     */
//    Message(MESSAGE_TYPE code, std::string username, std::string password, std::string path, std::string hash, std::vector<std::string> paths, std::unordered_map<std::string, std::string> hashes, std::vector<unsigned char> file, ElementStatus elementStatus, Comm_error *comm_error, bool status);

    /**
     * class constructor
     * @param code
     * @param username
     * @param password
     * @param path
     * @param hash
     * @param paths
     * @param hashes
     * @param file_string
     * @param elementStatus
     * @param comm_error
     * @param status
     */
//    Message(MESSAGE_TYPE code, const std::string& username, const std::string& password, std::string path, std::string hash, const std::vector<std::string>& paths, const std::unordered_map<std::string, std::string>& hashes, const std::string& file_string, ElementStatus elementStatus, Comm_error *comm_error, bool status);

    friend class boost::serialization::access;

public:
    MESSAGE_TYPE code;
    std::string username, password;
    std::string path, hash;
    std::vector<std::string> paths;
    std::unordered_map<std::string, std::string> hashes;
    std::vector<unsigned char> file;
    ElementStatus elementStatus;
    Comm_error *comm_error;
    bool status; // = okay

    /**
    * send this message
    * using a TLV format (type, length, value)
    * @return a list of asio buffers
    */
    [[nodiscard]] std::vector<boost::asio::const_buffer> send();

    /**
     * check if the inner status is okay
     * @return boolean
     */
    [[nodiscard]] bool is_okay() const;

    /**
     * class constructor
     * @param code
     */
    explicit Message(MESSAGE_TYPE code = MSG_ERROR);

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
    static Message *probe_content(const std::unordered_map <std::string, std::string> &hashes);

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
