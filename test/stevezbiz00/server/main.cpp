#include <iostream>
#include <list>
#include "Server.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        exit(-1);
    }
    try {
        boost::asio::io_context ctx;
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), std::stoi(argv[1]));
        Server server(ctx, endpoint);
        ctx.run();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
