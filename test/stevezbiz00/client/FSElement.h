//
// Created by stevezbiz on 02/11/20.
//

#ifndef BOOST_FS_FSELEMENT_H
#define BOOST_FS_FSELEMENT_H

#include <ctime>
#include <string>
#include <boost/filesystem.hpp>

enum class ElementType {
    file, dir
};

class FSElement {
    boost::filesystem::path path_;
    ElementType type_;
    time_t last_write_time_;
    std::string hash_;

    void SHA256();

public:
    FSElement(boost::filesystem::path path, ElementType type, time_t last_write_time);
    bool isOld(time_t lwt);
    bool needUpdate();

    ElementType getType();
};


#endif //BOOST_FS_FSELEMENT_H
