//
// Created by stevezbiz on 07/11/20.
//

#include "Server.h"

#include <utility>

Server::Server(boost::asio::io_context &ctx, const boost::asio::ip::tcp::endpoint &endpoint, std::string db_path,
               std::string root_path) : acceptor_(ctx, endpoint), socket_(ctx), db_(std::move(db_path)),
                                        root_path_(std::move(root_path)), stop_(false) {
    this->session_manager_ = std::make_shared<Session_manager>();
    this->api_ = std::make_unique<Server_API>(this->session_manager_);
    server_init();
    accept();
}

void Server::accept() {
    while (!stop_) {
        acceptor_.accept(socket_);
        std::thread thread([this](boost::asio::ip::tcp::socket socket) {
            if(!this->api_->run(std::make_unique<Socket_API>(std::move(socket), NO_RETRY, 500), SOCKET_TIMEOUT))
                Logger::error("Server::accept", "No correctly closing current socket");
        }, std::move(socket_));
        thread.detach();
    }
}

bool Server::login(Session *session, const std::string &username, const std::string &password,
                   const Database_API &database, const std::string &root_path) {
    if (database.login_query(username, password)) {
        Logger::info("Server::login", "Login correct: user " + username, PR_NORMAL);
        session->login_status = true;
        session->user=username;
        boost::filesystem::path dest_path{root_path};
        dest_path.append(username);
        if (!boost::filesystem::exists(dest_path)) {
            boost::filesystem::create_directory(dest_path);
        }
        return true;
    }
    Logger::info("Server::login", "Login failed", PR_NORMAL);
    return false;
}

const std::unordered_map<std::string, std::string> *Server::probe(Session *session, const std::vector<std::string> &paths, const Database_API &database) {
    return session->get_schema(database);
}

const std::vector<unsigned char> *Server::get(Session *session, const std::string &path, const std::string &root_path, ElementStatus &status) {
    boost::filesystem::path dest_path{root_path};
    dest_path.append(session->user);
    dest_path.append(path);
    if(boost::filesystem::is_regular_file(dest_path)){
        status=ElementStatus::createdFile;
        return new std::vector<unsigned char>(Utils::read_from_file(dest_path));
    } else {
        status=ElementStatus::createdDir;
        return new std::vector<unsigned char>();
    }
}

bool Server::push(Session *session, const std::string &path, const std::vector<unsigned char> &file,
                  const std::string &hash, ElementStatus status, const std::string &root_path, const Database_API &database) {
    boost::filesystem::path disk_path{root_path};
    disk_path.append(session->user);
    disk_path.append(path);
    boost::filesystem::path virt_path{session->user};
    virt_path.append(path);

    switch (status) {
        case ElementStatus::createdFile:
            Utils::write_on_file(disk_path, file);
            if (!session->create_file(database, virt_path.string(), hash)) {
                Logger::error("Server::push", "Cannot create file: " + disk_path.string(), PR_HIGH);
                return false;
            }
            break;
        case ElementStatus::modifiedFile:
            if (!boost::filesystem::remove(disk_path) || !Session::modify_file(database, virt_path.string(), hash)) {
                Logger::error("Server::push", "Cannot modify file: " + disk_path.string(), PR_HIGH);
                return false;
            }
            Utils::write_on_file(disk_path, file);
            break;
        case ElementStatus::erasedFile:
            if (!boost::filesystem::remove(disk_path) || !Session::remove_file(database, virt_path.string())) {
                Logger::error("Server::push", "Cannot erase file: " + disk_path.string(), PR_HIGH);
                return false;
            }
            break;
        case ElementStatus::createdDir:
            if (!boost::filesystem::create_directory(disk_path) || !session->create_dir(database, virt_path.string(), hash)) {
                Logger::error("Server::push", "Cannot create directory: " + disk_path.string(), PR_HIGH);
                return false;
            }
            break;
        case ElementStatus::erasedDir:
            if (!boost::filesystem::remove(disk_path) || !Session::remove_dir(database, virt_path.string())) {
                Logger::error("Server::push", "Cannot erase directory: " + disk_path.string(), PR_HIGH);
                return false;
            }
            break;
        default:
            return false;
    }
    return true;
}

const std::vector<std::string> *Server::restore(Session *session, const Database_API &database) {
    return session->get_path_schema(database);
}

bool Server::end(Session *session) {
    return true;
}

void Server::handle_error(Session *session, const Comm_error *comm_error) {
    Logger::error("Server::handle_error", comm_error->to_string(), PR_HIGH);
}

void Server::server_init() {
    api_->set_login([this](Session *session, const std::string &username, const std::string &password) {
        return login(session, username, password, db_, root_path_);
    });
    api_->set_end([](Session *session) {
        return end(session);
    });
    api_->set_get([this](Session *session, const std::string &path, ElementStatus &status) {
        return get(session, path, root_path_, status);
    });
    api_->set_probe([this](Session *session, const std::vector<std::string> &paths) {
        return probe(session, paths, db_);
    });
    api_->set_push([this](Session *session, const std::string &path, const std::vector<unsigned char> &file,
                         const std::string &hash, ElementStatus status) {
        return push(session, path, file, hash, status, root_path_, db_);
    });
    api_->set_restore([this](Session *session) {
        return restore(session, db_);
    });
    api_->set_handle_error([](Session *session, const Comm_error *comm_error) {
        return handle_error(session, comm_error);
    });
}
