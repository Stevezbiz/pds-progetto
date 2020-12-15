#include <iostream>
#include <thread>
#include <utility>
#include "Client.h"
#include "ConfigSetting.h"
#include "../comm/Logger.h"

int main(int argc, char **argv) {
    Logger::logger_filter = PR_NULL;

    try {
        ConfigSettings c;
        c.init_configuration();
        Client client{c.getDirPath(), c.getAddress(),c.getPort()};

        if(!client.pwdAttempts()){
            Logger::error("main", "No attempts remaining... terminate the program", PR_HIGH);
            client.close();
            exit(-1);
        }

        Command cmd = c.menu();
        switch (cmd) {
            case normal:
                if(!client.probe()){
                    // TODO: error management
                    Logger::error("main", "Probe failed", PR_HIGH);
                    client.close();
                    exit(-1);
                }
                break;
            case restore:
                if(!client.restore()) {
                    // TODO: error management
                    Logger::error("main", "Restore failed", PR_HIGH);
                    client.close();
                    exit(-1);
                }
                break;
            case end:
                return 0;
        }
        client.run();
        while(true){//If it triggered the third operation -> Quit
           if (c.question_menu() == 0) {
            if (c.question_yesno("Are you sure you want to exit?")) {
                std::cout << "Exit....\n";
                if(!client.close()){
                    Logger::error("main", "client cannot be closed", PR_HIGH);
                    exit(-1);
                }
                exit(0);
            }
           }
        }
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
