//
// Created by stevezbiz on 07/11/20.
//

#include "Server.h"

#include <utility>

Server::Server(boost::asio::io_context &ctx, const boost::asio::ip::tcp::endpoint &endpoint, std::string db_path,
               std::string root_path) : acceptor_(ctx, endpoint), socket_(ctx), api_(), db_(std::move(db_path)),
                                        root_path_(std::move(root_path)), stop_(false) {
    server_init();
    accept();
}

void Server::accept() {
    while (!stop_) {
        acceptor_.accept(socket_);
        std::thread thread([](Server_API &api, Session session) {
            api.run(session);
        }, std::ref(api_), std::move(socket_));
        thread.detach();
    }
}

bool Server::login(Session &session, const std::string &username, const std::string &password,
                   const Database_API& database) {
    if(database.login_query(username,password)){
        session.set_user(username);
        return true;
    }
    return false;
}

const std::map<std::string, std::string> *Server::probe(Session &session, const std::vector<std::string> &paths) {
    return new std::map<std::string, std::string>();
}

const std::vector<unsigned char> *Server::get(Session &session, const std::string &path) {
    return new std::vector<unsigned char>(Utils::read_from_file(path));
}

bool Server::push(Session &session, const std::string &path, const std::vector<unsigned char> &file,
                  const std::string &hash, ElementStatus status, const std::string &root_path) {
    boost::filesystem::path dest_path{root_path};
    dest_path.append(session.get_user());
    dest_path.append(path);
    switch (status) {
        case ElementStatus::createdFile:
            Utils::write_on_file(dest_path, file);
            break;
        case ElementStatus::modifiedFile:
            if (!boost::filesystem::remove(dest_path)) return false;
            Utils::write_on_file(path, file);
            break;
        case ElementStatus::erasedFile:
            if (!boost::filesystem::remove(dest_path)) return false;
            break;
        default:
            return false;
    }
    return true;
}

const std::vector<std::string> *Server::restore(Session &session) {
    return new std::vector<std::string>();
}

bool Server::end(Session &session) {
    return true;
}

void Server::handle_error(Session &session, const Comm_error *comm_error) {
    // Logger::error(comm_error);
    Logger::error("Server::handle_error", comm_error->to_string(), PR_HIGH);
    // std::cout << comm_error->to_string() << std::endl;
}

void Server::server_init() {
    api_.set_login([this](Session &session, const std::string &username, const std::string &password) {
        return login(session, username, password, db_);
    });
    api_.set_end([this](Session &session) {
        return end(session);
    });
    api_.set_get([this](Session &session, const std::string &path) {
        return get(session, path);
    });
    api_.set_probe([this](Session &session, const std::vector<std::string> &paths) {
        return probe(session, paths);
    });
    api_.set_push([this](Session &session, const std::string &path, const std::vector<unsigned char> &file,
                         const std::string &hash, ElementStatus status) {
        return push(session, path, file, hash, status, root_path_);
    });
    api_.set_restore([this](Session &session) {
        return restore(session);
    });
    api_.set_handle_error([this](Session &session, const Comm_error *comm_error) {
        return handle_error(session, comm_error);
    });
}
