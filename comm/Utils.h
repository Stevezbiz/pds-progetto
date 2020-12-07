//
// Created by Paolo Gastaldi on 24/11/2020.
//

#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include <iostream>
#include <vector>
#include <boost/filesystem.hpp>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>
#include "ElementStatus.h"

namespace fs = boost::filesystem;

class Utils {

public:
    /**
     * read a vector of bytes from a file
     * @param input path as a string
     * @return vector of bytes
     */
    static std::vector<unsigned char> read_from_file(const std::string &path);

    /**
     * read a vector of bytes from a file
     * @param input path
     * @return vector of bytes
     */
    static std::vector<unsigned char> read_from_file(const fs::path &path);

    /**
     * read a vector of bytes from a file
     * @param input stream
     * @return vector of bytes
     */
    static std::vector<unsigned char> read_from_file(std::istream &is);

    /**
     * write a vector of bytes on a file
     * @param output path as a string
     * @param vector of bytes
     */
    static void write_on_file(const std::string &path, const std::vector<unsigned char> &file);

    /**
     * write a vector of bytes on a file
     * @param output path
     * @param vector of bytes
     */
    static void write_on_file(const fs::path &path, const std::vector<unsigned char> &file);
    /**
     * write a vector of bytes on a file
     * @param output stream
     * @param vector of bytes
     */
    static void write_on_file(std::ostream &os, const std::vector<unsigned char> &file);

    /**
     *
     * @param path
     * @return
     */
    static std::string SHA256(const std::string &path);

    /**
     *
     * @param path
     * @return
     */
    static std::string SHA256(const boost::filesystem::path &path);

    /**
     *
     * @param input
     * @return
     */
    static std::string hash(const std::string &input);

};


#endif //SERVER_UTILS_H
