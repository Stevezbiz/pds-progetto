//
// Created by Paolo Gastaldi on 24/11/2020.
//

#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include <iostream>
#include <vector>

class Utils {
public:
    static std::vector<unsigned char> read_from_file(std::istream &is);

    static void write_on_file(std::ostream &os, const std::vector<unsigned char> &file);
};


#endif //SERVER_UTILS_H
