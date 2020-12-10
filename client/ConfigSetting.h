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
    /**
     * Function to used to initialize the application
     */
    void init_configuration();
    /**
     * If a init_config file already exist -> read the initial configuration from it
     */
    void read_config();
    /**
     * If an init_config file doesn't exist or the user wants to overwrite -> initialize initial variables and create the file
     */
    void write_config();
    /**
     * Check if a file already exists
     */
    static inline bool exist_file (const std::string& name);
    /**
     * Manage the creation of yes and no questions
     */
    static bool question_yesno(std::string const& message);
    /**
     * Convert every answer into a boolean
     */
    static bool checkAnswerOK(std::string& answer, bool& result);
    /**
     * Show the configuration of the server
     */
    void showConfig();
    /**
     * Manage the menu and all the possible answers
     */
    void menu();
    /**
     * Manage the answers of the menu
     */
    int question_menu();
    /**
     * Show the the choices from menu
     */
    void showChoice();

    std::string getAddress();

    int getPort();

    boost::filesystem::path getDirPath();
};



