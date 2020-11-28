#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <boost/filesystem.hpp>
#include <utility>
class ConfigSettings
{
    std::string address;
    boost::filesystem::path dir_path;
    int port=0;

public:
    ConfigSettings() = default;
    ConfigSettings(std::string address,boost::filesystem::path dir_path,int port):address(std::move(address)),dir_path(std::move(dir_path)),port(port){};

    void read_config() {
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

    static inline bool exist_file (const std::string& name) {
        if (FILE *file = fopen(name.c_str(), "r")) {
            fclose(file);
            return true;
        } else {
            return false;
        }
    }

    void write_config() {
        char choice;
        boost::system::error_code ec;
        std::string work_directory;
        do{
            std::cout << "Address: " << std::flush;
            std::getline(std::cin, address);
            boost::asio::ip::address::from_string(address, ec);
            if (ec) {
                std::cerr << "The ip address is uncorrect!\n" << std::endl;
            }
            std::this_thread::sleep_for (std::chrono::milliseconds(50));
        }while (ec);
        // Get the port.
        std::cout << "Port: " << std::flush;
        std::cin >> port;

        while(true) {
            std::cout << "Insert the folder path to perform the remote backup: ";
            std::cin >> work_directory ;

            dir_path = boost::filesystem::path(work_directory);

            if (!boost::filesystem::is_directory(dir_path) || !boost::filesystem::exists(dir_path)) { // Check if src folder exists
                std::cout << "The folder doesn't not exist" << std::endl;
                std::cout << "Would you like to create the folder?(Y/N): ";

                std::cin >> choice;
                if (choice == 'Y' || choice == 'y') {
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
    static bool checkAnswerOK(std::string& answer, bool& result)
    {
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


    static bool question_yesno(std::string const& message)
    {
        std::string answer;
        bool        result;

        std::cout << message << "? [Y/n] ";
        while(std::cin >> answer && !checkAnswerOK(answer, result))
        {
            std::cout << "Invalid answer: " << answer << " Please try again\n"
                      << message << "? [Y/n] ";
        }
        if (!std::cin) {
            // We never got an answer.
            // Not much we can do here. Probably give up?
            throw std::runtime_error("User Input read failed");
        }
        return result;
    }
    void init_configuration(){
        bool exist_config = ConfigSettings::exist_file("init_config.txt");
        std::cout << R"(
         ___ ___ __  __  ___ _____ ___   ___   _   ___ _  ___   _ ___
        | _ \ __|  \/  |/ _ \_   _| __| | _ ) /_\ / __| |/ / | | | _ \
        |   / _|| |\/| | (_) || | | _|  | _ \/ _ \ (__| ' <| |_| |  _/
        |_|_\___|_|  |_|\___/ |_| |___| |___/_/ \_\___|_|\_\\___/|_|
        )" << '\n';
        while(true) {
            bool new_config = question_yesno("A configuration file already exists, do you want to use that");
            if (exist_config) {

                if (new_config) {
                    read_config();
                    break;
                }

            }
            if (!exist_config || !new_config) {
                std::cin.ignore(1);
                write_config();
                break;
            }
        }

    };

};




