//
// Created by Paolo Gastaldi on 28/11/2020.
//

#include <iostream>
#include <thread>
#include "Stub_client.h"

int main(int argc, char **argv) {
    if (argc != 6) {
        std::cerr << "Usage: " << argv[0] << " <directory> <host> <port> <username> <password>" << std::endl;
        exit(-1);
    }

    Stub_client client{argv[1], argv[2], argv[3]};
    if (!client.login(argv[4], argv[5]))
        exit(-2);
    else
        std::cout << "Success: " << argv[4] << " logged in" << std::endl;

    client.normal();
    // client.restore();

    return 0;
}
