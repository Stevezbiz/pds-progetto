#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <boost/filesystem.hpp>
#include <utility>

class ServerConfigSetting {

        boost::filesystem::path dir_path_ = "./server_files";
        int port_ = 8080;
        std::string logs_path_ = ".server_logs.txt";
        std::string db_path_ = "../database/remote_backup.db";

    public:
        ServerConfigSetting() = default;
        /**
         *
         * @param address
         * @param dir_path
         * @param port
         * @param logs_path
         */
        ServerConfigSetting( boost::filesystem::path dir_path, int port, std::string logs_path, std::string db_path);

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
        static inline bool exist_file(const std::string &name);

        /**
         * Manage the creation of yes and no questions
         */
        static bool question_yesno(std::string const &message);

        /**
         * Convert every answer into a boolean
         */
        static bool check_answer_ok(std::string &answer, bool &result);

        /**
         * Show the configuration of the server
         */
        void show_config();

        /**
         *
         * @return
         */
        [[nodiscard]] std::string get_port() const;

        /**
         *
         * @return
         */
        std::string get_dir_path();

        /**
         *
         * @return
         */
        std::string get_logs_path();

        /**
         *
         * @return
         */
        std::string get_db_path();

};
