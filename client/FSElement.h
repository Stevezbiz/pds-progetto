//
// Created by stevezbiz on 02/11/20.
//

#ifndef BOOST_FS_FSELEMENT_H
#define BOOST_FS_FSELEMENT_H

#include <ctime>
#include <string>
#include <boost/filesystem.hpp>

class FSElement {
    boost::filesystem::path path_;
    time_t last_write_time_;
    std::string hash_;

public:
    FSElement(boost::filesystem::path path, time_t last_write_time);

    bool isOld(time_t lwt);

    bool needUpdate();
};


#endif //BOOST_FS_FSELEMENT_H
