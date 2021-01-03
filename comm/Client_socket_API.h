//
// Created by Paolo Gastaldi on 26/11/2020.
//

#ifndef PDS_PROGETTO_CLIENT_SOCKET_API_H
#define PDS_PROGETTO_CLIENT_SOCKET_API_H

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/system/system_error.hpp>
#include <utility>
#include "Socket_API.h"

constexpr int MAX_THREADS = 4;
constexpr int WAIT_TIMEOUT = 1000 * 60 * 5;

class Client_socket_API : public Socket_API {
    int thread_id_{ 0 };
    std::map<int, std::future<bool>> threads_;
    std::mutex m_threads_;
    std::condition_variable cv_;
    std::atomic<int> n_active_threads_{ 0 };

    /**
     * check if this is a connection error status
     * @return status
     */
    bool is_connection_error_();

    /**
     * create a new API based on the current one
     * @return new client socket API
     */
    std::shared_ptr<Client_socket_API> create_new_API_();

protected:
    std::string cookie_;

public:
    Client_socket_API() = delete;

    /**
     * class constructor
     * @param ip
     * @param port
     * @param keep_alive - it could be deprecated in future
     */
    Client_socket_API(std::string ip, std::string port, bool keep_alive = false);

    /**
     * (sync) send a message and retrieve the response
     * @param message
     * @param expected_message
     * @param retry_cont_ - please do not use it!
     * @return status
     */
    bool send_and_receive(const std::shared_ptr<Message> &message, MESSAGE_TYPE expected_message, int retry_cont_ = 0);

    /**
     * async send a message and retrieve the response
     * this function will block you only if no more threads are available
     * @param message
     * @param expected_message
     * @param cb
     * @return status
     */
    bool async_send_and_receive(const std::shared_ptr<Message> &message, MESSAGE_TYPE expected_message, const std::function<bool(bool, const std::shared_ptr<Message> &, const std::shared_ptr<Comm_error> &)> &);

    /**
     * wait all async functions previously called
     * @return true if all async functions returned true, false otherwise
     */
    bool wait_all_async();
};


#endif //PDS_PROGETTO_CLIENT_SOCKET_API_H
