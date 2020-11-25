//
// Created by Paolo Gastaldi on 24/11/2020.
//

#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include <iostream>
#include <vector>

class Utils {
public:
    /**
     * read a vector of bytes from a file
     * @param input stream
     * @return vector of bytes
     */
    static std::vector<unsigned char> read_from_file(std::istream &is) {
        return std::vector<unsigned char>{ std::istreambuf_iterator<char>(is), {} };
    }

    /**
     * write a vector of bytes on a file
     * @param output stream
     * @param vector of bytes
     */
    static void write_on_file(std::ostream &os, const std::vector<unsigned char> &file) {
        auto file_buffer = reinterpret_cast<char *>(file[0]); // TODO: check this
        os << file_buffer;
    }
};


#endif //SERVER_UTILS_H
