//
// Created by stevezbiz on 07/11/20.
//

#include "Server.h"

#include <utility>

namespace fs = boost::filesystem;

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
        std::unique_lock<std::mutex> lock(this->m_threads_);
        this->cv_.wait(lock, [this]() { return this->n_active_threads_ < MAX_THREADS; });
        this->n_active_threads_++;
        std::thread thread([this](boost::asio::ip::tcp::socket socket) {
            if (!this->api_->run(std::make_unique<Socket_API>(std::move(socket), NO_RETRY, 500), SOCKET_TIMEOUT))
                Logger::error("Server::accept", "No correctly closing current socket");
            this->n_active_threads_--;
            this->cv_.notify_one();
        }, std::move(socket_));
        thread.detach();
        lock.unlock();
    }
}

// TODO: evaluate this alternative, discard the request when all server threads are busy
//void Server::accept() {
//    while (!stop_) {
//        acceptor_.accept(socket_);
//        if(this->n_active_threads_ >= MAX_THREADS) { // if all threads are busy, discard the request
//            std::thread thread([this](boost::asio::ip::tcp::socket socket) {
//                Logger::warning("Server::accept", "Discard request");
//                if (!this->api_->discard(std::make_unique<Socket_API>(std::move(socket))))
//                    Logger::error("Server::accept", "No correctly closing current socket");
//            }, std::move(socket_));
//            continue;
//        }
//        this->n_active_threads_++;
//        std::thread thread([this](boost::asio::ip::tcp::socket socket) {
//            if (!this->api_->run(std::make_unique<Socket_API>(std::move(socket), NO_RETRY, 500), SOCKET_TIMEOUT))
//                Logger::error("Server::accept", "No correctly closing current socket");
//            this->n_active_threads_--;
//            this->cv_.notify_one();
//        }, std::move(socket_));
//        thread.detach();
//    }
//}

bool Server::login(Session *session, const std::string &username, const std::string &password,
                   const Database_API &database, const std::string &root_path) {
    if (database.login_query(username, password)) {
        Logger::info("Server::login", "Login correct: user " + username, PR_NORMAL);
        session->login_status = true;
        session->user = username;
        fs::path dest_path{root_path};
        dest_path.append(username);
        if (!fs::exists(dest_path)) {
            fs::create_directory(dest_path);
        }
        return true;
    }
    Logger::info("Server::login", "Login failed", PR_NORMAL);
    return false;
}

const std::unordered_map<std::string, std::string> *Server::probe(Session *session,
                                                                  const std::vector<std::string> &paths,
                                                                  const Database_API &database) {
    Logger::info("Server::probe", "Probing server content", PR_LOW);
    return session->get_schema(database);
}

const std::vector<unsigned char> *Server::get(Session *session, const std::string &path, const std::string &root_path,
                                              ElementStatus &status) {
    fs::path dest_path{root_path};
    dest_path.append(session->user);
    dest_path.append(path);
    if (fs::is_regular_file(dest_path)) {
        status = ElementStatus::createdFile;
        Logger::info("Server::get", "File " + dest_path.string(), PR_LOW);
        return new std::vector<unsigned char>(Utils::read_from_file(dest_path));
    } else {
        status = ElementStatus::createdDir;
        Logger::info("Server::get", "Directory " + dest_path.string(), PR_LOW);
        return new std::vector<unsigned char>();
    }
}

bool Server::push(Session *session, const std::string &path, const std::vector<unsigned char> &file,
                  const std::string &hash, ElementStatus status, const std::string &root_path,
                  const Database_API &database) {
    fs::path disk_path{root_path};
    disk_path.append(session->user);
    if (status == ElementStatus::createdFile || status == ElementStatus::createdDir)
        create_dirs(disk_path, path);
    disk_path.append(path);
    fs::path virt_path{session->user};
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
            if (!Session::modify_file(database, virt_path.string(), hash)) {
                Logger::error("Server::push", "Cannot modify file: " + disk_path.string(), PR_HIGH);
                return false;
            }
            Utils::write_on_file(disk_path, file);
            break;
        case ElementStatus::erasedFile:
            if (fs::exists(disk_path)) {
                fs::remove(disk_path);
            }
            if (!Session::remove_file(database, virt_path.string())) {
                Logger::error("Server::push", "Cannot erase file: " + disk_path.string(), PR_HIGH);
                return false;
            }
            break;
        case ElementStatus::createdDir:
            if (!fs::exists(disk_path)) {
                fs::create_directory(disk_path);
            }
            if (!session->create_dir(database, virt_path.string(), hash)) {
                Logger::error("Server::push", "Cannot create directory: " + disk_path.string(), PR_HIGH);
                return false;
            }
            break;
        case ElementStatus::erasedDir:
            if (fs::exists(disk_path)) {
                fs::remove_all(disk_path);
            }
            if (!Session::remove_dir(database, virt_path.string())) {
                Logger::error("Server::push", "Cannot erase directory: " + disk_path.string(), PR_HIGH);
                return false;
            }
            break;
        default:
            return false;
    }
    Logger::info("Server::push", "Success: " + disk_path.string(), PR_LOW);
    return true;
}

const std::vector<std::string> *Server::restore(Session *session, const Database_API &database) {
    Logger::info("Server::probe", "Getting server content", PR_LOW);
    return session->get_path_schema(database);
}

bool Server::end(Session *session) {
    Logger::info("Server::end", "Closing session", PR_LOW);
    return true;
}

void Server::handle_error(Session *session, const Comm_error *comm_error) {
    Logger::error(comm_error);
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

void Server::create_dirs(fs::path base, const std::string &path) {
    fs::path dirs(path);
    dirs.remove_leaf();
    fs::path dir(std::move(base));
    for (const auto &p : dirs) {
        dir.append(p.string());
        if (!fs::exists(dir))
            fs::create_directory(dir);
    }
}
