#include <iostream>
#include <boost/filesystem.hpp>
#include <string>
#include "FileWatcher.h"

namespace fs = boost::filesystem;

void simple_ls(const fs::path &dir_path) {
    if (!fs::exists(dir_path)) {
        std::cerr << "Directory " << dir_path.leaf() << " not found" << std::endl;
        return;
    }
    if (!fs::is_directory(dir_path)) {
        std::cerr << dir_path.string() << " is not a directory" << std::endl;
        return;
    }
    fs::directory_iterator it(dir_path);
    int count = 0;
    for (const auto &el:it) {
        std::cout << el.path().leaf() << std::endl;
        count++;
    }
    std::cout << std::endl;
    std::cout << count << " total" << std::endl;
}

void fs_exercise() {
//    if (argc != 2) {
//        std::cerr << "Usage: " << argv[0] << " file/directory" << std::endl;
//        exit(-1);
//    }
//    fs::path dir_path{argv[1]};
//    simple_ls(dir_path);
    std::string cmd;
    std::cout << "> ls ";
    std::cin >> cmd;
    while (cmd != "exit") {
        fs::path dir_path{cmd};
        simple_ls(dir_path);
        std::cout << "> ls ";
        std::cin >> cmd;
    }
}

void action(std::string path_to_watch, ElementStatus status) {
    if (!fs::is_regular_file(fs::path(path_to_watch)) &&
        !fs::is_directory(fs::path(path_to_watch)) &&
        status != ElementStatus::erasedFile &&
        status != ElementStatus::erasedDir)
        return;
    switch (status) {
        case ElementStatus::erasedFile:
            std::cout << "File erased: " << path_to_watch << std::endl;
            break;
        case ElementStatus::createdFile:
            std::cout << "File created: " << path_to_watch << std::endl;
            break;
        case ElementStatus::modifiedFile:
            std::cout << "File modified: " << path_to_watch << std::endl;
            break;
        case ElementStatus::createdDir:
            std::cout << "Directory created: " << path_to_watch << std::endl;
            break;
        case ElementStatus::erasedDir:
            std::cout << "Directory erased: " << path_to_watch << std::endl;
            break;
        default:
            std::cerr << "Error! Unknown file status" << std::endl;
    }
}

void file_watcher() {
    FileWatcher fw{".", std::chrono::milliseconds(1000)};
    fw.start(action);
}

int main(int argc, char **argv) {
    file_watcher();
    return 0;
}
