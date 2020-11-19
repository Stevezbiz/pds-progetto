//
// Created by stevezbiz on 07/11/20.
//

#include "Server.h"
#include <iostream>
#include <boost/filesystem.hpp>
/**
 * Inizializza i campi dell'oggetto e pone il server in attesa di accettare connessioni
 * @param ctx: i servizi di I/O forniti
 * @param endpoint: l'indirizzo e la porta a cui lanciare il server
 */
Server::Server(boost::asio::io_context &ctx, const boost::asio::ip::tcp::endpoint &endpoint)
        : acceptor_(ctx, endpoint), socket_(ctx) {

    std::cout << "Server started\n";

    //set as example a working dir
    m_workDirectory = "User1";
    create_work_dir();

    do_accept();
}

/**
 * Accetta le connessioni dei client
 */
void Server::do_accept() {
    acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
        if (!ec) {
            std::make_shared<Session>(std::move(socket_))->start();
        }
        do_accept();
    });
}

void Server::create_work_dir()
{
    using namespace boost::filesystem;
    //set as currentPath for file destination a m_workDirectory folder spottend in current directory
    auto parent = boost::filesystem::current_path().parent_path();
    auto currentPath = (parent /= path(m_workDirectory));

    std::cout << "..creating "<< m_workDirectory << " directory" << std::endl;

    if (!is_directory(currentPath) || !exists(currentPath)) { // Check if src folder exists
        boost::filesystem::create_directories(currentPath); // create src folder
    }

    //set the current directory as the dir passed as parameter so the
    //session work on this folder
    current_path(currentPath);

}