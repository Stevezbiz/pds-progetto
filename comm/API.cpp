//
// Created by stevezbiz on 26/11/20.
//

#include "API.h"

namespace fs = boost::filesystem;

/**
 * class constructor
 * @param socket_api
 * @param root_path
 */
explicit API::API(Socket_API &socket_api, const std::string &root_path = ".") : api_(socket_api), root_path_(root_path) {}

/**
 * save a file on disk
 * the path is a relative path from the root_path
 * @param message
 * @return status
 */
bool API::_save_file(Message &message) {
    fs::path full_path = this->root_path_ / fs::path(message->path_);
    fs::ofstream os{full_path};

    char *file_buffer = reinterpret_cast<char *>(&(message->file_[0]));
    os << file_buffer;
    os.close();

    return true;
}