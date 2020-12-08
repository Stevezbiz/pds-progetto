#include <iostream>
#include <thread>
#include <utility>
#include "Client.h"
#include "ConfigSetting.h"

int main(int argc, char **argv) {
    try {
        ConfigSettings c;
        c.init_configuration();
        boost::asio::io_context ctx;
        boost::asio::ip::tcp::resolver resolver(ctx);
        auto endpoint_iterator = resolver.resolve({c.getAddress(), c.getPort()});
        boost::asio::ip::tcp::socket socket{ctx};
        boost::asio::connect(socket, endpoint_iterator);
        Client client{c.getDirPath(), std::move(socket)};
        std::thread t([&ctx]() {
            ctx.run();
        });

        if(!client.pwdAttempts()){
            Logger::info("main", "No attempts remaining... terminate the program", PR_HIGH);
            t.join();
            client.close();
            exit(-1);
        }

        Command cmd = c.menu();
        switch (cmd) {
            case normal:
                if(!client.probe()){
                    // TODO: error management
                }
                break;
            case restore:
                if(!client.restore()){
                    // TODO: error management
                }
                break;
            case end:
                return 0;
        }
        client.run();
        if(!client.close()){
            // TODO: error management
        }
        t.join();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
