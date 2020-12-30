//
// Created by carlo on 28/11/20.
//

#include "ConfigSetting.h"

ConfigSettings::ConfigSettings(std::string address, boost::filesystem::path dir_path, int port, std::string logs_path)
        : address_(std::move(address)), dir_path_(std::move(dir_path)), port_(port), logs_path_(std::move(logs_path)) {}


void ConfigSettings::read_config() {
    std::ifstream config(".client_config.txt");
    std::string line;
    while (getline(config, line)) {
        std::istringstream sin(line.substr(line.find('=') + 1));
        if (line.find("address") != -1)
            sin >> address_;
        else if (line.find("port") != -1)
            sin >> port_;
        else if (line.find("path") != -1)
            sin >> dir_path_;
        else if (line.find("logs") != -1)
            sin >> logs_path_;
    }
    show_config();
}

bool ConfigSettings::exist_file(const std::string &name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

void ConfigSettings::write_config() {
    boost::system::error_code ec;
    std::string address;
    std::string port;
    std::string dir_path;
    std::string logs_path;

    // get the address
    do {
        std::cout << "Server address (default '" + address_ + "'): " << std::flush;
        std::getline(std::cin, address);
        if (!address.empty()) {
            boost::asio::ip::address::from_string(address, ec);
            if (ec) {
                std::cout << "The ip address is invalid!" << std::endl;
                std::cin.clear();
            } else {
                address_ = address;
            }
            //std::this_thread::sleep_for (std::chrono::milliseconds(40));
        }
    } while (ec);

    // get the port
    for (;;) {
        std::cout << "Port (default '" + std::to_string(port_) + "'): ";
        std::getline(std::cin, port);
        if (!port.empty()) {
            if ((std::stoi(port) > 0 ? (int) log10((double) std::stoi(port)) + 1 : 1) == 4) {
                port_ = std::stoi(port);
                break;
            } else {
                std::cout << "Please enter a valid port." << std::endl;
                std::cin.clear();
                std::cin.ignore(1);
            }
        } else
            break;
    }
    // get the directory to bakup
    while (true) {
        std::cout << "Folder to backup (default '" + dir_path_.string() + "'): ";
        std::getline(std::cin, dir_path);
        if (!dir_path.empty()) {
            if (boost::filesystem::exists(dir_path)) {
                if (boost::filesystem::is_directory(dir_path)) {
                    // the directory already exists
                    dir_path_ = boost::filesystem::path(dir_path);
                    break;
                } else {
                    // a file with directory's name already exists
                    std::cout << "Cannot name the folder: file '" + dir_path + "' already exists" << std::endl;
                }
            } else {
                // the directory does not exists
                std::cout << "The folder doesn't not exist" << std::endl;
                if (question_yesno("Would you like to create the folder")) {
                    boost::filesystem::create_directory(dir_path); // create src folder
                    dir_path_ = boost::filesystem::path(dir_path);
                    std::cout << "The " << dir_path << " folder has been created." << std::endl;
                    break;
                }
            }
        } else {
            break;
        }
    }

    // get the logs path
    while (true) {
        std::cout << "Insert the path to save logs (default '" + logs_path_ + "'): ";
        std::getline(std::cin, logs_path);
        if (!logs_path.empty()) {
            if (boost::filesystem::is_directory(logs_path_)) {
                // a directory with file's name already exists
                std::cout << "Cannot name the file: folder '" + logs_path + "' already exists" << std::endl;
            } else {
                logs_path_ = logs_path;
                break;
            }
        } else {
            break;
        }
    }

    // Create and open a text file
    std::ofstream conf(".client_config.txt");

    // Write to the file
    conf << "address = " << address_ << std::endl;
    conf << "port = " << port_ << std::endl;
    conf << "path = " << dir_path_ << std::endl;
    conf << "logs = " << logs_path_ << std::endl;

    // Close the file
    conf.close();
    show_config();
}

bool ConfigSettings::question_yesno(const std::string &message) {
    std::string answer;
    bool result;

    std::cout << message << "? [Y/n] ";
    while (std::cin >> answer && !check_answer_ok(answer, result)) {
        std::cout << "Invalid answer: " << answer << " ... Please try again\n"
                  << message << "? [Y/n] ";
    }
    if (!std::cin) {
        // We never got an answer.
        // Not much we can do here. Probably give up?
        throw std::runtime_error("User Input read failed");
    }

    return result;
}

bool ConfigSettings::check_answer_ok(std::string &answer, bool &result) {
    std::transform(answer.begin(), answer.end(), answer.begin(), [](unsigned char x) { return ::tolower(x); });

    bool answer_valid =
            (answer == "y") ||
            (answer == "n") ||
            (answer == "yes") ||
            (answer == "no") ||
            (answer == "q") ||
            (answer == "quit");
    result = answer_valid && answer[0] == 'y';
    return answer_valid;
}

void ConfigSettings::init_configuration() {
    bool exist_config = ConfigSettings::exist_file(".client_config.txt");
    std::cout << R"(
         ___ ___ __  __  ___ _____ ___   ___   _   ___ _  ___   _ ___
        | _ \ __|  \/  |/ _ \_   _| __| | _ ) /_\ / __| |/ / | | | _ \
        |   / _|| |\/| | (_) || | | _|  | _ \/ _ \ (__| ' <| |_| |  _/
        |_|_\___|_|  |_|\___/ |_| |___| |___/_/ \_\___|_|\_\\___/|_|
        )" << '\n';
    while (true) {
        if (exist_config) {

            if (question_yesno("A configuration file already exists, do you want to use that")) {
                read_config();
                break;
            } else {
                //used to ignore the N/n caracther to the next input
                std::cin.ignore(1);
            }

        }
        write_config();
        break;
    }
}

std::string ConfigSettings::get_address() {
    return this->address_;
}

std::string ConfigSettings::get_port() const {
    return std::to_string(port_);
}

std::string ConfigSettings::get_dir_path() {
    return this->dir_path_.string();
}

Command ConfigSettings::menu() {
    //unsigned short response = 0;
    unsigned short response;
    while (true) {
        show_choice();
        response = question_menu();
        switch (response) {
            case 0:
                if (question_yesno("Are you sure you want to exit?")) {
                    std::cout << "Closing...\n";
                    return Command::end;
                }
                break;
            case 1:
                std::cout << "Synchronizing with server...\n";
                return Command::normal;
            case 2:
                if (question_yesno("Are you sure to recover all files and folders from the Server")) {
                    std::cout << "Restoring..." << std::endl;
                    return Command::restore;
                }
                break;
            default:
                std::cout << "Unrecognized command\n";
        }
    }

}

void ConfigSettings::show_config() {
    std::cout << R"(
    +---------------------------------------+
    |  Address = )" << address_ << R"(
    |  Port = )" << port_ << R"(
    |  Path = )" << dir_path_ << R"(
    |  Logs = )" << logs_path_ << R"(
    +---------------------------------------+
        )" << '\n';
}

void ConfigSettings::show_choice() {
    std::cout
            << "-------------------------------------\nWhat action would you like to take?\n-------------------------------------\n"
            << "  1) Normal mode (keeps the remote backup server updated of any changes)\n"
            << "  2) Restore all (recover all files and folders from Remote Backup Server)\n"
            << "  q) Exit\n"
            << ">> : ";
}

int ConfigSettings::question_menu() {
    std::string answer;
    bool result;

    while (std::cin >> answer && (!check_answer_ok(answer, result) && (answer != "1" && answer != "2"))) {
        std::cout << "Invalid answer: " << answer << " ... Please try again\n:";
    }
    if (!std::cin) {
        // We never got an answer.
        // Not much we can do here. Probably give up?
        throw std::runtime_error("User Input read failed");
    }
    if (isdigit(answer[0])) return std::stoi(answer);
    return 0;
}

std::string ConfigSettings::get_logs_path() {
    return logs_path_;
}


