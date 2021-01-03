#include <iostream>
#include "Server.h"
#include "ServerConfigSetting.h"

#define LOG_FILE ".server_logs.txt"

int main(int argc, char **argv) {
    ServerConfigSetting c;
    c.init_configuration();
    Logger::logger_filter = PR_VERY_LOW;
    std::ofstream log_stream;
//    log_stream.open(LOG_FILE, std::ios_base::app); // append
    log_stream.open(c.get_logs_path(), std::ios::out | std::ios::trunc); // erase previous content
    Logger::redirect(log_stream);

    try {

        boost::asio::io_context ctx;
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), std::stoi(c.get_port()));
        Server server(ctx, endpoint, c.get_db_path(), c.get_dir_path());
        ctx.run();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    log_stream.close();
    return 0;
}
