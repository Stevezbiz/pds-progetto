#include <iostream>
#include <thread>
#include <utility>
#include "Client.h"
#include "ConfigSetting.h"
#include "../comm/Logger.h"

#define LOG_FILE ".client_logs.txt"

int main(int argc, char **argv) {
    Logger::logger_filter = PR_LOW;
    std::ofstream log_stream;

    Client *client;
    try {
        ConfigSettings c;
        c.init_configuration();
        log_stream.open(LOG_FILE, std::ios_base::app); // append
        log_stream.open(c.get_logs_path(), std::ios::out | std::ios::trunc); // erase previous content
        Logger::redirect(log_stream);
        client = new Client{c.get_dir_path(), c.get_address(), c.get_port()};

        if (!client->pwdAttempts()) {
            Logger::error("main", "No attempts remaining... terminate the program", PR_HIGH);
            log_stream.close();
            exit(-1);
        }

        Command cmd = ConfigSettings::menu();
        switch (cmd) {
            case normal:
                if (!client->probe()) {
                    // TODO: error management
                    Logger::error("main", "Probe failed", PR_HIGH);
                    client->close();
                    log_stream.close();
                    exit(-1);
                }
                break;
            case restore:
                if (!client->restore()) {
                    // TODO: error management
                    Logger::error("main", "Restore failed", PR_HIGH);
                    client->close();
                    log_stream.close();
                    exit(-1);
                }
                break;
            case end:
                return 0;
        }
        client->run();
        while (true) {//If it triggered the third operation -> Quit
            if (ConfigSettings::question_menu() == 0) {
                if (ConfigSettings::question_yesno("Are you sure you want to exit?")) {
                    if (!client->close()) {
                        Logger::error("main", "client cannot be closed", PR_HIGH);
                        log_stream.close();
                        exit(-1);
                    }
                    break;
                }
            }
        }
    } catch (std::runtime_error &re) {
        std::cerr << "Exception: " << re.what() << std::endl;
        client->close();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        client->close();
    }
    log_stream.close();
    return 0;
}
