//
// Created by carlo on 28/11/20.
//

#include "ConfigSetting.h"

void ConfigSettings::read_config() {
    std::ifstream config("init_config.txt");
    std::string line;
    while (getline(config, line)) {
        std::istringstream sin(line.substr(line.find('=') + 1));
        if (line.find("address") != -1)
            sin >> address;
        else if (line.find("port") != -1)
            sin >> port;
        else if (line.find("path") != -1)
            sin >> dir_path;
    }
    std::cout<<"address = "<<address<<"\nport = "<<port << "\npath = "<< dir_path<<std::endl;
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
    std::string work_directory;
    do{
        std::cout << "Address: " << std::flush;
        std::getline(std::cin, address);
        boost::asio::ip::address::from_string(address, ec);
        if (ec) {
            std::cout << "The ip address is invalid!" << std::endl;
            std::cin.clear();
        }
        //std::this_thread::sleep_for (std::chrono::milliseconds(40));
    }while (ec);
    // Get the port.
    for (;;) {
        std::cout << "Port: ";
        if (std::cin >> port && ((port > 0 ? (int) log10 ((double) port) + 1 : 1 ) == 4)) {
            break;
        } else {
            std::cout << "Please enter a valid port." << std::endl;
            std::cin.clear();
            std::cin.ignore(1);
        }
    }
    //insert working directory
    while(true) {
        std::cout << "Insert the folder path to perform the remote backup: ";
        std::cin >> work_directory ;

        dir_path = boost::filesystem::path(work_directory);

        if (!boost::filesystem::is_directory(dir_path) || !boost::filesystem::exists(dir_path)) { // Check if src folder exists
            std::cout << "The folder doesn't not exist" << std::endl;

            if (question_yesno("Would you like to create the folder")) {
                std::cout << "The " << work_directory << " folder is created."<<std::endl;
                boost::filesystem::create_directory(dir_path); // create src folder
                break;
            }

        }else {
            break;
        }
    }

    // Create and open a text file
    std::ofstream conf("init_config.txt");

    // Write to the file
    conf << "address = " << address << std::endl;
    conf << "port = " << port << std::endl;
    conf << "path = " << dir_path << std::endl;
    // Close the file
    conf.close();
}

bool ConfigSettings::question_yesno(const std::string &message) {
    std::string answer;
    bool        result;

    std::cout << message << "? [Y/n] ";
    while(std::cin >> answer && !checkAnswerOK(answer, result))
    {
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

bool ConfigSettings::checkAnswerOK(std::string &answer, bool &result) {
    std::transform(answer.begin(), answer.end(), answer.begin(),
                   [](unsigned char x){return ::tolower(x);});

    bool answer_valid =
            (answer == "y")   ||
            (answer == "n")   ||
            (answer == "yes") ||
            (answer == "no");

    result = answer_valid && answer[0] == 'y';
    return answer_valid;
}

void ConfigSettings::init_configuration() {
    bool exist_config = ConfigSettings::exist_file("init_config.txt");
    std::cout << R"(
         ___ ___ __  __  ___ _____ ___   ___   _   ___ _  ___   _ ___
        | _ \ __|  \/  |/ _ \_   _| __| | _ ) /_\ / __| |/ / | | | _ \
        |   / _|| |\/| | (_) || | | _|  | _ \/ _ \ (__| ' <| |_| |  _/
        |_|_\___|_|  |_|\___/ |_| |___| |___/_/ \_\___|_|\_\\___/|_|
        )" << '\n';
    while(true) {
        if (exist_config) {

            if (question_yesno("A configuration file already exists, do you want to use that")) {
                read_config();
                break;
            }else{
                //used to ignore the N/n caracther to the next input
                std::cin.ignore(1);
            }

        }
        write_config();
        break;
    }
}

std::string ConfigSettings::getAddress() {
    return this->address;
}

int ConfigSettings::getPort() {
    return this->port;
}

boost::filesystem::path ConfigSettings::getDirPath() {
    return this->dir_path;
}
