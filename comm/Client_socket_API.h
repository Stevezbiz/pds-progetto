//
// Created by Paolo Gastaldi on 26/11/2020.
//

#ifndef PDS_PROGETTO_CLIENT_SOCKET_API_H
#define PDS_PROGETTO_CLIENT_SOCKET_API_H

class Client_socket_API : public Socket_API {

public:
    /**
     * class constructor
     * @param socket
     */
    explicit Socket_API(boost::asio::io_service& socket) : Socket_API(socket) {}

    /**
     * create a new client socket API
     * it automatically creates a new socket
     * @param ip
     * @param port
     * @param socket_timeout (milliseconds)
     */
    static Client_socket_API new(std::string ip, std::string port, int socket_timeout) {
        boost::asio::io_context ctx;
        boost::asio::ip::tcp::resolver resolver{ ctx };
        auto endpoint_iterator = resolver.resolve({ ip, port }); // set ip, port
        boost::asio::ip::tcp::socket socket{ io_context }; // create socket

        connect(socket, std::move(endpoint_iterator), [this](boost::system::error_code ec) { });

        return new Client_socket_API(socket);
    }
};


#endif //PDS_PROGETTO_CLIENT_SOCKET_API_H
