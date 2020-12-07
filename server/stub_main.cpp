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
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), std::stoi(argv[3]));
    boost::asio::ip::tcp::acceptor acceptor{ctx, endpoint};
    boost::asio::ip::tcp::socket socket{ctx};
    acceptor.accept(socket);
    // auto api = new Server_API{new Socket_API{std::move(socket)}};
    auto api = new Server_API{};
    // api->set_end([](const Session &session) { return true; });
    api->set_login([](const Session &session, const std::string &username, const std::string &password) {
        return username == "Bellofigo";
    });
    api->set_push([](const Session &session, const std::string &path,const std::vector<unsigned char> &file,const std::string &hash,ElementStatus es) {
        std::ostringstream file_stream;
        for(unsigned char c: file) {
            file_stream << c;
        }
        std::string file_string{ file_stream.str() };
        std::cout << file_string << std::endl;
        return true;
    });
    std::thread t([&ctx]() {
        ctx.run();
    });
    Session session{ std::move(socket) };
    api->run(session);
    t.join();
    return 0;
}
