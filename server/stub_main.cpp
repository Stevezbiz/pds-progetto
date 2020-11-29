//
// Created by Paolo Gastaldi on 28/11/2020.
//

#include <iostream>
#include "../comm/Server_API.h"

int main(int argc, char **argv) {
    if (argc != 6) {
        std::cerr << "Usage: " << argv[0] << " <directory> <host> <port> <username> <password>" << std::endl;
        exit(-1);
    }

    boost::asio::io_context ctx;
    boost::asio::ip::tcp::resolver resolver(ctx);
    auto endpoint_iterator = resolver.resolve({argv[2], argv[3]});
    boost::asio::ip::tcp::socket socket{ctx};
    boost::asio::connect(socket, endpoint_iterator);

    auto api = new Server_API{new Socket_API{std::move(socket)}};

    Server_API::set_end([]() { return true; });
    Server_API::set_login([](const std::string &username, const std::string &password) {
        return username == "Bellofigo";
    });
    api->run();

    return 0;
}
