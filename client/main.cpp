#include <iostream>
#include <thread>
#include <utility>
#include "Client.h"
#include "FileWatcher.h"
#include "ConfigSetting.h"

/**
 * Il file watcher viene avviato e lancerà le azioni da compiere nell'handler
 * @param client: l'oggetto Client che si occupa di inviare messaggi al server
 * @param directory: la directory che il file watcher deve monitorare
 */
void startFileWatcher(Client &client, std::string directory) {
    FileWatcher fw{std::move(directory), std::chrono::milliseconds(1000)};
    fw.start([&client](const std::string &path_to_watch, ElementStatus status) -> void {
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
    /*if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <directory> <host> <port>" << std::endl;
        exit(-1);
    }*/
    try {
        ConfigSettings c;
        c.init_configuration();
        boost::asio::io_context ctx;
        boost::asio::ip::tcp::resolver resolver(ctx);
        auto endpoint_iterator = resolver.resolve({c.getAddress(), c.getPort()});
        Client client(ctx, endpoint_iterator);
        std::thread t([&ctx]() {
            ctx.run();
        });
        startFileWatcher(client, argv[3]);
        client.close();
        t.join();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
