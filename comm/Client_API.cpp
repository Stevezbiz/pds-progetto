//
// Created by stevezbiz on 26/11/20.
//

#include <set>
#include <utility>
#include "Client_API.h"

namespace fs = boost::filesystem;

bool Client_API::get_and_save_(const std::string &path) {
    api_->async_send_and_receive(Message::get(path),MSG_GET_CONTENT,
       [this, path](bool status, const std::shared_ptr<Message> &res, const std::shared_ptr<Comm_error> &comm_error) {
           Logger::info("Client_API::get_and_save_", "Analyzing new file...", PR_VERY_LOW);
           if (!res->is_okay()) {
               Logger::error(comm_error.get());
               return false;
           }
           fs::path dest_path{root_path_};
           dest_path.append(path);
           res->path = dest_path.string();
           std::unique_lock<std::mutex> lock(m_);
           cv_.wait(lock, [this] () { return !active_thread_; });
           active_thread_=true;
           lock.unlock();
           try{
               create_dirs(root_path_, path);
               if(res->elementStatus==ElementStatus::createdFile){
                   if (!Client_API::save_file_(res)){
                       Logger::error("Client_API::get_and_save_","Cannot save the file " + path);
                       return false;
                   }
                   Logger::info("Client_API::get_and_save_", "Restored file '" + path + "'", PR_NORMAL);
                   std::cout << "Restored file '" + path + "'" << std::endl;
               } else if(!fs::exists(dest_path)) {
                   if(!fs::create_directory(dest_path))
                       Logger::error("Client_API::get_and_save_","Cannot save the directory " + path);
                   Logger::info("Client_API::get_and_save_", "Restored directory '" + path + "'", PR_NORMAL);
                   std::cout << "Restored directory '" + path + "'" << std::endl;
               }
           } catch( std::exception &e) {
               std::cout << "Unexpected error while restoring '" << path << "'" << std::endl;
               Logger::error("Client_API::get_and_save_",e.what());
               active_thread_=false;
               cv_.notify_one();
               return false;
           }
           active_thread_=false;
           cv_.notify_one();
           Logger::info("Client_API::get_and_save_", "Analyzing new file... - done", PR_VERY_LOW);

           return true;
       });

//    if(!api_->send_and_receive(Message::get(path), MSG_GET_CONTENT))
//        return false;
//    auto res = api_->get_message();
//    if (!res->is_okay())
//        return false;
//    fs::path dest_path{root_path_};
//    dest_path.append(path);
//    res->path = dest_path.string();
//    if(res->elementStatus==ElementStatus::createdFile){
//        if (!Client_API::save_file_(res))
//            return false;
//    } else {
//        fs::create_directory(dest_path);
//    }

    return true;
}

Client_API::Client_API(Client_socket_API *socket_api, std::string root_path) : API(socket_api), api_(socket_api), root_path_(std::move(root_path)) {}

bool Client_API::login(const std::string &username, const std::string &password) {
    Logger::info("Client_API::login", "Trying to perform login...", PR_LOW);
    if(!api_->send_and_receive(Message::login(username, password), MSG_OKAY))
        return false;
    auto res = api_->get_message();
    Logger::info("Client_API::login", "Trying to perform login... - done", PR_LOW);

    return res->is_okay();
}

bool Client_API::probe(const std::map<std::string, std::string> &map) {
    Logger::info("Client_API::probe", "Probe check started...", PR_LOW);
    bool sent = false;
    std::vector<std::string> paths;
    paths.reserve(map.size());
    for (auto const &item : map)
        paths.push_back(item.first);
    if(!api_->send_and_receive(Message::probe(paths), MSG_PROBE_CONTENT))
        return false;
    auto res = api_->get_message();

    if(!res->is_okay())
        return false;

    for(auto const &item : res->hashes) {
        auto path = item.first;
        Logger::info("Client_API::probe", "Analyzing " + path + " from server...", PR_VERY_LOW);
        fs::path dest_path{root_path_};
        dest_path /= path ;
        auto it = map.find(path);

        auto hash = item.second;
        if(it == map.end()) {
            if(!hash.empty()) { // file
                // file not found -> erase
                Logger::info("Client_API::probe", "Deleted file found " + path, PR_LOW);
                if (!push(std::vector<unsigned char>(), path, "", ElementStatus::erasedFile, -1))
                    return false;
            }
            else { // dir
                // dir not found -> erase
                Logger::info("Client_API::probe", "Deleted dir found " + path, PR_LOW);
                if (!push(std::vector<unsigned char>(), path, "", ElementStatus::erasedDir, -1))
                    return false;
            }
            sent = true;
        }
    }

    // if the client has a different file than the server, the client starts a push procedure
    for(auto const &item : map) {
        auto path = item.first;
        Logger::info("Client_API::probe", "Analyzing " + path + "...", PR_VERY_LOW);
        auto it = res->hashes.find(path);
        fs::path dest_path{root_path_};
//        dest_path.append(path);
        dest_path /= path ;
        if(fs::is_regular_file(dest_path)){
            if(it==res->hashes.end()){
                // file not registered -> create
                Logger::info("Client_API::probe", "Created file found " + path, PR_LOW);
                if (!push(Utils::read_from_file(dest_path), path, map.at(path), ElementStatus::createdFile, -1))
                    return false;
                sent = true;
            } else if(res->hashes.at(path)!=item.second){
                // different hash -> modified
                Logger::info("Client_API::probe", "Modified file found " + path, PR_LOW);
                if (!push(Utils::read_from_file(dest_path), path, map.at(path),ElementStatus::modifiedFile, -1))
                    return false;
                sent = true;
            }
        } else if(fs::is_directory(dest_path)){
            if(it==res->hashes.end()){
                // dir not registered -> create
                Logger::info("Client_API::probe", "Created dir found " + path, PR_LOW);
                if (!push(std::vector<unsigned char>(), path, map.at(path), ElementStatus::createdDir, -1))
                    return false;
                sent = true;
            }
        }
    }
    bool status;
    if(sent)
        status = api_->wait_all_async();
    else {
        api_->send_and_receive(Message::okay(), MSG_OKAY);
        status = api_->get_message()->is_okay();
    }
    Logger::info("Client_API::probe", "Probe check started... - done", PR_LOW);
    return status;
}

bool Client_API::push(const std::vector<unsigned char> &file, const std::string &path, const std::string &hash, ElementStatus elementStatus, int fw_cycle) {
    Logger::info("Client_API::push", "Push started...", PR_LOW);

    if(fw_cycle_ != fw_cycle) {
        Logger::info("Client_API::push", "New filewatcher cycle, waiting all threads...", PR_VERY_LOW);
        if (!api_->wait_all_async()) // all files from the previous filewatcher analysy must be pushed before proceding
            return false;
        fw_cycle_ = fw_cycle;
        Logger::info("Client_API::push", "New filewatcher cycle, waiting all threads... - done", PR_VERY_LOW);
    }

    api_->async_send_and_receive(Message::push(file, path, hash, elementStatus), MSG_OKAY,
        [path, elementStatus](bool status, const std::shared_ptr<Message> &res, const std::shared_ptr<Comm_error> &comm_error) {
            Logger::info("Client_API::push", "Analyzing new file...", PR_VERY_LOW);
            if (!status || !res->is_okay()){
                Logger::error(comm_error.get());
                return false;
            }
            Logger::info("Client_API::push", "Analyzing new file... - done", PR_VERY_LOW);
            switch(elementStatus){
                case ElementStatus::createdDir:
                    Logger::info("Client::run", "Created directory '" + path + "' on server", PR_NORMAL);
                    std::cout << "Created directory '" + path + "' on server" << std::endl;
                    break;
                case ElementStatus::createdFile:
                    Logger::info("Client::run", "Created file '" + path + "' on server", PR_NORMAL);
                    std::cout << "Created file '" + path + "' on server" << std::endl;
                    break;
                case ElementStatus::erasedDir:
                    Logger::info("Client::run", "Erased directory '" + path + "' from server", PR_NORMAL);
                    std::cout << "Erased directory '" + path + "' from server" << std::endl;
                    break;
                case ElementStatus::erasedFile:
                    Logger::info("Client::run", "Erased file '" + path + "' from server", PR_NORMAL);
                    std::cout << "Erased file '" + path + "' from server" << std::endl;
                    break;
                case ElementStatus::modifiedFile:
                    Logger::info("Client::run", "Modified file '" + path + "' on server", PR_NORMAL);
                    std::cout << "Modified file '" + path + "' on server" << std::endl;
                    break;
            }
            return res->is_okay();
        });

//    if(!api_->send_and_receive(Message::push(file, path, hash, elementStatus),MSG_OKAY))
//        return false;
//    auto res = api_->get_message();
    Logger::info("Client_API::push", "Push started... - done");

//    return res->is_okay();
    return true;
}

bool Client_API::restore() {
    fs::remove_all(root_path_);
    fs::create_directory(root_path_);
    if(!api_->send_and_receive(Message::restore(),MSG_RESTORE_CONTENT))
        return false;
    auto res = api_->get_message();

    if(!res->is_okay())
        return false;

    std::set<std::string> paths;
    for(const auto& el : res->paths){
        paths.insert(el);
    }
    for(const auto &path : paths){
        if (!get_and_save_(path))
            return false;
    }

    return api_->wait_all_async();
}

bool Client_API::end() {
    Logger::info("Client_API::end", "End started...", PR_LOW);
    api_->wait_all_async(); // thread safe
    if(!api_->send_and_receive(Message::end(), MSG_OKAY))
        return false;
    auto res = api_->get_message();
    Logger::info("Client_API::end", "End started... - done", PR_LOW);
    return res->is_okay();
}

void Client_API::create_dirs(fs::path base, const std::string &path) {
    fs::path dirs(path);
    dirs.remove_leaf();
    fs::path dir(std::move(base));
    for (const auto &p : dirs) {
        dir.append(p.string());
        if (!fs::exists(dir))
            fs::create_directory(dir);
    }
}