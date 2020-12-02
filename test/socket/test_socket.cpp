//
// Created by Paolo Gastaldi on 02/12/2020.
//

#include <iostream>
#include <sstream>
#include <utility>
#include <vector>
#include <map>
#include <utility>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/bind/bind.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <stdexcept>

std::string default_ip{ "127.0.0.1" };
std::string default_port{ "8080" };
constexpr size_t BUFFER_SIZE = 10;

class Client {
    boost::asio::ip::tcp::socket *socket_;

public:
    Client(const std::string &ip, const std::string &port) {
        std::cerr << "(Client::Client) connect..." << std::endl;
        try {
            boost::asio::io_context ctx;
            boost::asio::ip::tcp::resolver resolver(ctx);
            auto endpoint_iterator = resolver.resolve({ ip, port });
            this->socket_ = new boost::asio::ip::tcp::socket{ ctx };
            boost::asio::connect(*this->socket_, endpoint_iterator);
        } catch(const std::exception &e) {
            std::cerr << "(Client::Client) connect error: " << e.what() << std::endl;
            return;
        }
        std::cerr << "(Client::Client) connect... - done " << std::endl;
    }

    void start() {
        boost::system::error_code ec;

        // --- write ----
        std::cerr << "(Client::start) write..." << std::endl;
        std::string req{ "test" };
        std::ostringstream req_stream{};
        req_stream << std::setw(BUFFER_SIZE) << req;
        auto req_data = req_stream.str();
        std::vector<boost::asio::const_buffer> req_buffers{};
        req_buffers.emplace_back(boost::asio::buffer(req_data, BUFFER_SIZE));

        boost::asio::write(*this->socket_, req_buffers, ec);
        if(ec)
            std::cerr << "(Client::start) write error: " << ec << std::endl;
        else
            std::cerr << "(Client::start) write... - done" << std::endl;

        // --- read ----
        std::cerr << "(Client::start) read..." << std::endl;
        char res_ary[BUFFER_SIZE];
        auto res_buffer = boost::asio::mutable_buffer(res_ary, BUFFER_SIZE);
        boost::asio::read(*this->socket_, res_buffer, ec);
        if(ec)
            std::cerr << "(Client::start) read error: " << ec << std::endl;

        std::istringstream is{ std::string{ res_ary, 4 }};
        std::string res;
        if(!(is >> res))
            std::cerr << "(Client::start) cannot read response content" << std::endl;
        else
            std::cerr << "(Client::start) read... - done" << std::endl;

        std::cerr << "(Client::start) done: " << res << std::endl;
   }

    ~Client() {
        if(this->socket_->is_open())
            this->socket_->close();
        delete this->socket_;
    }
};

class Server {
    boost::asio::ip::tcp::socket *socket_;
    boost::asio::ip::tcp::acceptor *acceptor_;
    std::string port_;

public:
    Server(std::string port) : port_(std::move(port)) {};

    void start() {
        try {
            boost::asio::io_context ctx;
            boost::asio::ip::tcp::endpoint endpoint{ boost::asio::ip::tcp::v4(), static_cast<unsigned short>(std::stoi(this->port_)) };
            this->socket_ = new boost::asio::ip::tcp::socket{ ctx };
            this->acceptor_ = new boost::asio::ip::tcp::acceptor{ ctx, endpoint };

            std::cerr << "(Server::start) accept..." << std::endl;
            this->acceptor_->async_accept(*this->socket_, [this](boost::system::error_code ec) {
                if(!ec) {
                    std::cerr << "(Server::start) accept error: " << ec << std::endl;
                    return;
                }
                else
                    std::cerr << "(Server::start) accept... - done" << std::endl;

                this->run(); // accept only once
            });
            ctx.run();
        } catch (std::exception &e) {
            std::cerr << "(Server::start) " << e.what() << std::endl;
            exit(-1);
        }
    }

    void run() {
        try {
            boost::system::error_code ec;

            // --- read ----
            std::cerr << "(Server::run) read..." << std::endl;
            char req_ary[BUFFER_SIZE];
            auto req_buffer = boost::asio::mutable_buffer(req_ary, BUFFER_SIZE);
            boost::asio::read(*this->socket_, req_buffer, ec);
            if(ec)
                std::cerr << "(Server::run) read error: " << ec << std::endl;

            std::istringstream is{ std::string{ req_ary, 4 }};
            std::string req;
            if(!(is >> req))
                std::cerr << "(Server::run) cannot read request content" << std::endl;
            else
                std::cerr << "(Server::run) read... - done" << std::endl;

            // --- write ----
            std::cerr << "(Server::run) write..." << std::endl;
            std::string res{ "test done" };
            std::ostringstream res_stream{};
            res_stream << std::setw(BUFFER_SIZE) << res;
            auto req_data = res_stream.str();
            std::vector<boost::asio::const_buffer> res_buffers{};
            res_buffers.emplace_back(boost::asio::buffer(req_data, BUFFER_SIZE));

            boost::asio::write(*this->socket_, res_buffers, ec);
            if(ec)
                std::cerr << "(Server::run) write error: " << ec << std::endl;
            else
                std::cerr << "(Server::run) write... - done" << std::endl;

            std::cerr << "(Server::run) done: " << res << std::endl;
        }
        catch (std::exception &e) {
            std::cerr << "(Server::run) " << e.what() << std::endl;
            exit(-1);
        }
    }

    ~Server() {
        if(this->socket_->is_open())
            this->socket_->close();
        delete this->socket_;
        delete this->acceptor_;
    }
};

int main(int argc, char *argv[0]) {
    if(argc != 2) {
        std::cerr << "USAGE: " + std::string{ argv[0] } + " <client|server>" << std::endl;
        exit(-1);
    }

    auto mode = std::string{ argv[1] };
    std::cout << "Running mode: " << mode << std::endl;
    Client *client;
    Server *server;
    if(!mode.compare("client")) {
        client = new Client{ default_ip, default_port };
        client->start();
    }
    else if(!mode.compare("server")) {
        server = new Server{ default_port };
        server->start();
    }
    else {
        std::cerr << "ERROR: wrong mode" << std::endl;
        exit(-1);
    }

    delete client;
    delete server;

    return 0;
};