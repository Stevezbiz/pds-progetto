//
// Created by stevezbiz on 26/11/20.
//

#include "Utils.h"

std::vector<unsigned char> Utils::read_from_file(const std::string &path) {
    return Utils::read_from_file(fs::path{ path });
}

std::vector<unsigned char> Utils::read_from_file(const fs::path &path) {
    auto is = fs::ifstream{ path };
    auto file = Utils::read_from_file(is);
    is.close();
    return file;
}

std::vector<unsigned char> Utils::read_from_file(std::istream &is) {
    return std::vector < unsigned char > { std::istreambuf_iterator<char>(is), {} };
}

void Utils::write_on_file(const std::string &path, const std::vector<unsigned char> &file) {
    Utils::write_on_file(fs::path{ path }, file);
}

void Utils::write_on_file(const fs::path &path, const std::vector<unsigned char> &file) {
    auto os = fs::ofstream{ path };
    Utils::write_on_file(os, file);
    os.close();
}

void Utils::write_on_file(std::ostream &os, const std::vector<unsigned char> &file) {
    auto file_buffer = reinterpret_cast<char *>(file[0]); // TODO: check this
    os << file_buffer;
}
