#include <iostream>
#include "Server.h"

#define LOG_FILE ".server_logs.txt"

int main(int argc, char **argv) {
    Logger::logger_filter = PR_VERY_LOW;
    std::ofstream log_stream;
//    log_stream.open(LOG_FILE, std::ios_base::app); // append
    log_stream.open(LOG_FILE, std::ios::out | std::ios::trunc); // erase previous content
    Logger::redirect(log_stream);

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

    log_stream.close();
    return 0;
}
