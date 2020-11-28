//
// Created by stevezbiz on 26/11/20.
//

#include "Utils.h"
#include <openssl/sha.h>
#include <iomanip>

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
/**
 * Calcola l'hash di un file utilizzando SHA256 implementato in openssl
 * Per uniformità viene calcolato anche l'hash per una directory, utilizzando il suo nome
 */
std::string Utils::SHA256(const boost::filesystem::path &path) {
    std::ifstream fp(path.string(), std::ios::in | std::ios::binary);
    if (!fp.good()) {
        std::ostringstream os;
        os << "Cannot open " << path << " :" << std::strerror(errno);
        throw std::runtime_error(os.str());
    }
    std::size_t buf_size{4096};
    char buf[buf_size];
    unsigned char digest[SHA256_DIGEST_LENGTH] = {0};

    SHA256_CTX ctx;
    SHA256_Init(&ctx);

    while (fp.good()) {
        fp.read(buf, buf_size);
        SHA256_Update(&ctx, buf, fp.gcount());
    }
    fp.close();

    SHA256_Final(digest, &ctx);

    std::ostringstream os;
    os << std::hex << std::setfill('0');
    for (unsigned char i : digest) {
        os << std::setw(2) << i;
    }
    return os.str();
}