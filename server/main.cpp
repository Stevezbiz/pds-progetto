#include <iostream>
#include "Server.h"

int main(int argc, char **argv) {
    Logger::logger_filter = PR_NULL;

    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <port> <root directory> <database directory>" << std::endl;
        exit(-1);
    }
    try {
        boost::asio::io_context ctx;
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), std::stoi(argv[1]));
        Server server(ctx, endpoint, argv[3], argv[2]);
        ctx.run();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
