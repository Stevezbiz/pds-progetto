#include <iostream>
#include <thread>
#include "Client.h"
#include "FileWatcher.h"

namespace fs = boost::filesystem;

void file_watcher(Client &client) {
    FileWatcher fw{".", std::chrono::milliseconds(1000)};
    fw.start([&client](const std::string& path_to_watch, ElementStatus status) -> void {
        std::ostringstream os;
        switch (status) {
            case ElementStatus::erasedFile:
                os << "File erased: " << path_to_watch;
                break;
            case ElementStatus::createdFile:
                os << "File created: " << path_to_watch;
                break;
            case ElementStatus::modifiedFile:
                os << "File modified: " << path_to_watch;
                break;
            case ElementStatus::createdDir:
                os << "Directory created: " << path_to_watch;
                break;
            case ElementStatus::erasedDir:
                os << "Directory erased: " << path_to_watch;
                break;
            default:
                std::cerr << "Error! Unknown file status" << std::endl;
                exit(-1);
        }
        Message message{};
        message.setBody(os.str().data());
        message.encodeHeader();
        client.write(message);
    });
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <host> <port>" << std::endl;
        exit(-1);
    }
    try {
        boost::asio::io_context ctx;
        boost::asio::ip::tcp::resolver resolver(ctx);
        auto endpoint_iterator = resolver.resolve({argv[1], argv[2]});
        Client client(ctx, endpoint_iterator);
        std::thread t([&ctx](){
            ctx.run();
        });
        file_watcher(client);
        client.close();
        t.join();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
