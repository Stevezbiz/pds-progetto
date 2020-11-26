//
// Created by stevezbiz on 26/11/20.
//

#include "Utils.h"

/**
 * read a vector of bytes from a file
 * @param input stream
 * @return vector of bytes
 */
staticstd::vector<unsigned char> Utils::read_from_file(std::istream &is) {
    return std::vector < unsigned char > {std::istreambuf_iterator<char>(is), {}};
}

/**
 * write a vector of bytes on a file
 * @param output stream
 * @param vector of bytes
 */
static void Utils::write_on_file(std::ostream &os, const std::vector<unsigned char> &file) {
    auto file_buffer = reinterpret_cast<char *>(file[0]); // TODO: check this
    os << file_buffer;
}