//
// Created by stevezbiz on 26/11/20.
//

#include "Socket_API.h"

using namespace boost::asio;

/**
     * testing handler
     * @param ec
     * @param bytes_transferred
     */
static void Socket_API::generic_handler(const boost::system::error_code &ec, std::size_t bytes_transferred) {
    std::cout << ec << " errors, " << bytes_transferred << " bytes as been transferred correctly" << std::endl;
}

/**
     * constructor
     * @param socket
     */
explicit Socket_API::Socket_API(io_context &socket) : socket_(socket) {}

/**
     * socket setter
     * @param socket
     */
void Socket_API::set_socket(ip::tcp::socket &&socket) {
    if (this->socket_.is_open())
        this->socket_.close();
    this->socket_ = std::move(socket);
}

/**
     * socket getter
     * @return socket
     */
ip::tcp::socket &&Socket_API::get_socket() {
    return std::move(this->socket_);
}

/**
     * send a Message
     * (sync mode by default)
     * wait until the message as been sent
     * @tparam Handler
     * @param message
     * @param handler
     */
template<typename Handler>
void Socket_API::send(Message *message = new Message{ERROR}, Handler handler = generic_handler) {
    write(this->socket_, message->send(), handler);
}

/**
 * async send a Message
 * @tparam Handler
 * @param message
 * @param handler
 */
template<typename Handler>
void Socket_API::async_send(Message *message = new Message{ERROR}, Handler handler = generic_handler) {
    async_write(this->socket_, message->send(), handler); // deferred or async ? The system chooses
}

/**
     * receive a message
     * @tparam Handler
     * @param expectedMessage
     * @param handler
     * @return message
     */
template<typename Handler>
Message *Socket_API::receive(Message *expectedMessage = new Message{ERROR}, Handler handler = generic_handler) {
    auto message = new Message{};
    read(this->socket_, message->get_header_buffer(), handler);

    message->build(); // build the header
    if (message->code != expectedMessage->code)
        return new Message{ERROR};

    read(socket_, message->get_content_buffer(), handler);

    return message->build(); // build the whole message
}

Socket_API::~Socket_API() {
    if (this->socket_.is_open())
        this->socket_.close();
}