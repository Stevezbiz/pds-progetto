#include "ServerConfigSetting.h"

#include <utility>

namespace fs = boost::filesystem;

ServerConfigSetting::ServerConfigSetting(fs::path dir_path, int port, std::string logs_path, std::string db_path)
        : dir_path_(std::move(dir_path)), port_(port),
          logs_path_(std::move(logs_path)),
          db_path_(std::move(db_path)) {}


void ServerConfigSetting::init_configuration() {
    bool exist_config = ServerConfigSetting::exist_file(".server_config.txt");
    std::cout << "Server started..." << std::endl;
    //create the default file on server if it is not present
    if (!fs::exists(dir_path_)) {
        fs::create_directory(dir_path_); // create src folder
    }
    while (true) {
        if (exist_config) {

            if (question_yesno("A configuration file already exists, do you want to use that")) {
                read_config();
                break;
            } else {
                //used to ignore the N/n character to the next input
                std::cin.ignore(1);
            }

        }
        write_config();
        break;
    }
}

bool ServerConfigSetting::question_yesno(const std::string &message) {
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

bool ServerConfigSetting::check_answer_ok(std::string &answer, bool &result) {
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

void ServerConfigSetting::read_config() {
    std::ifstream config(".server_config.txt");
    std::string line;
    while (getline(config, line)) {
        std::istringstream sin(line.substr(line.find('=') + 1));
        if (line.find("port") != -1)
            sin >> port_;
        else if (line.find("path") != -1)
            sin >> dir_path_;
        else if (line.find("logs") != -1)
            sin >> logs_path_;
        else if (line.find("db") != -1)
            sin >> db_path_;
    }
    show_config();
}

void ServerConfigSetting::write_config() {
    std::string port;
    std::string dir_path;
    std::string logs_path;
    std::string db_path;
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
    // get the directory to backup
    while (true) {
        std::cout << "Folder to backup (default '" + dir_path_.string() + "'): ";
        std::getline(std::cin, dir_path);
        if (!dir_path.empty()) {
            if (fs::exists(dir_path)) {
                if (fs::is_directory(dir_path)) {
                    // the directory already exists
                    dir_path_ = fs::path(dir_path);
                    break;
                } else {
                    // a file with directory's name already exists
                    std::cout << "Cannot name the folder: file '" + dir_path + "' already exists" << std::endl;
                }
            } else {
                // the directory does not exists
                std::cout << "The folder doesn't not exist" << std::endl;
                if (question_yesno("Would you like to create the folder")) {
                    fs::create_directory(dir_path); // create src folder
                    dir_path_ = fs::path(dir_path);
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
            if (fs::is_directory(logs_path_)) {
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

    // get the logs path
    while (true) {
        std::cout << "Insert the path of DB (default '" + db_path_ + "'): ";
        std::getline(std::cin, db_path);
        if (!db_path.empty()) {
            if (fs::is_directory(db_path_)) {
                // a directory with file's name already exists
                std::cout << "Cannot name the file: folder '" + db_path + "' already exists" << std::endl;
            } else {
                db_path_ = db_path;
                break;
            }
        } else {
            break;
        }
    }

    // Create and open a text file
    std::ofstream conf(".server_config.txt");

    // Write to the file
    conf << "port = " << port_ << std::endl;
    conf << "path = " << dir_path_ << std::endl;
    conf << "logs = " << logs_path_ << std::endl;
    conf << "db = " << db_path_ << std::endl;
    // Close the file
    conf.close();
    show_config();
}

void ServerConfigSetting::show_config() {
    std::cout << R"(
    +---------------------------------------+
    |  Port = )" << port_ << R"(
    |  Path = )" << dir_path_ << R"(
    |  Logs = )" << logs_path_ << R"(
    |  DB = )" << db_path_ << R"(
    +---------------------------------------+
        )" << '\n';
}

bool ServerConfigSetting::exist_file(const std::string &name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

std::string ServerConfigSetting::get_port() const {
    return std::to_string(port_);
}

std::string ServerConfigSetting::get_dir_path() {
    return this->dir_path_.string();
}

std::string ServerConfigSetting::get_logs_path() {
    return logs_path_;
}

std::string ServerConfigSetting::get_db_path() {
    return db_path_;
}


