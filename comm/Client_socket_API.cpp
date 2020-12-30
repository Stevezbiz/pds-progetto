//
// Created by Paolo Gastaldi on 07/12/2020.
//

#include "Client_socket_API.h"

bool Client_socket_API::is_connection_error_() {
    Logger::info("Client_socket_API::is_connection_error_", this->get_last_error()->original_ec.message(), PR_VERY_LOW);
    std::vector errors{ // possible errors
            boost::asio::error::not_connected,
            boost::asio::error::timed_out,
            boost::asio::error::shut_down,
            boost::asio::error::fault
    };
    return std::any_of(errors.begin(), errors.end(),
                       [this](auto error) { return this->get_last_error()->original_ec == error; });
}

std::shared_ptr<Client_socket_API> Client_socket_API::create_new_API_() {
    auto new_API = std::make_unique<Client_socket_API>(this->ip, this->port, false);
    new_API->keep_alive_ = false;
    new_API->cookie_ = this->cookie_;
    new_API->retry_delay_ = this->retry_delay_;
    new_API->n_retry_ = this->n_retry_;
    return new_API;
}

Client_socket_API::Client_socket_API(std::string ip, std::string port, bool keep_alive) :
        Socket_API(std::move(ip), std::move(port), RETRY_ONCE, 500, keep_alive) {}

bool Client_socket_API::send_and_receive(const std::shared_ptr<Message> &message, MESSAGE_TYPE expected_message, int retry_cont_) {
    Logger::info("Client_socket_API::send_and_receive", "Sending a message...", PR_VERY_LOW);

    if (!this->open_conn())
        return false;
    Logger::info("Client_socket_API::send_and_receive", "Set cookie...", PR_VERY_LOW);
    message->cookie = this->cookie_;
    auto ret_val = this->send(message);

    if (!ret_val)
        return false;
    Logger::info("Client_socket_API::send_and_receive", "Sending a message... - done", PR_VERY_LOW);

    Logger::info("Client_socket_API::send_and_receive", "Receiving a message...", PR_VERY_LOW);
    ret_val = this->receive(expected_message);
    if (!ret_val) {
        if (this->is_connection_error_()) { // if connection timeout
            Logger::info("Client_socket_API::send_and_receive", "Open the connection again and retry...", PR_VERY_LOW);
            if (!this->open_conn(true))
                return false;
            ret_val = this->send(message);
            if (!ret_val)
                return false;
            ret_val = this->receive(expected_message);
        }
        else if (this->get_message() && this->get_message()->code == MSG_RETRY_LATER && retry_cont_ <= this->n_retry_) { // check if the server is busy
            std::this_thread::sleep_for(std::chrono::milliseconds(retry_delay_));
            Logger::warning("Client_socket_API::send_and_receive", "Server is busy and asked to retry later");
            ret_val = this->send_and_receive(message, expected_message, retry_cont_ + 1);
        }
        else
            return false;
    }
    if (ret_val) {
        Logger::info("Client_socket_API::send_and_receive", "Get cookie...", PR_VERY_LOW);
        if(this->get_message()->cookie != COOKIE_KEEP_THE_SAME)
            this->cookie_ = this->get_message()->cookie;
    }

    if (!this->close_conn())
        return false;
    Logger::info("Client_socket_API::send_and_receive", "Receiving a message... - done", PR_VERY_LOW);
    return ret_val;
}

bool Client_socket_API::async_send_and_receive(const std::shared_ptr<Message> &message, MESSAGE_TYPE expected_message, const std::function<bool(bool, const std::shared_ptr<Message> &, const std::shared_ptr<Comm_error> &)> &cb) {
    Logger::info("Client_socket_API::async_send_and_receive", "Creating new thread...", PR_VERY_LOW);

    std::unique_lock<std::mutex> lock(this->m_threads_);
    this->cv_.wait(lock, [this]() { return this->n_active_threads_ < MAX_THREADS; });

    auto thread_id = this->thread_id_++;
    this->n_active_threads_++;
    this->threads_.emplace(thread_id, std::async(std::launch::async, [this, message, expected_message, cb](int thread_id, std::shared_ptr<Client_socket_API> api) {
           Logger::info("Client_socket_API::async_send_and_receive", "Thread started, id " + std::to_string(thread_id), PR_VERY_LOW);
           auto ret_val = api->send_and_receive(message, expected_message);
           Logger::info("Client_socket_API::async_send_and_receive", "Thread connection done, id " + std::to_string(thread_id), PR_VERY_LOW);
           if(!api->get_message())
               this->comm_error = api->get_last_error();
           ret_val = cb(ret_val, api->get_message(), api->get_last_error());
           Logger::info("Client_socket_API::async_send_and_receive", "Thread callback done, id " + std::to_string(thread_id), PR_VERY_LOW);
           this->n_active_threads_--;
           this->cv_.notify_one();
           return ret_val;
       }, thread_id, this->create_new_API_()));

    lock.unlock();
    Logger::info("Client_socket_API::async_send_and_receive", "Creating new thread... - done", PR_VERY_LOW);

    return true;
}

bool Client_socket_API::wait_all_async() {
    bool status = true;
    std::lock_guard<std::mutex> lg(this->m_threads_);

    Logger::info("Client_socket_API::wait_all_async", "Waiting all threads...", PR_VERY_LOW);
    auto it = this->threads_.begin();
    while (it != this->threads_.end()) {
        if (!it->second.get()) {
            status = false;
        }
        it = this->threads_.erase(it);
    }
    this->thread_id_ = 0;

    Logger::info("Client_socket_API::wait_all_async", "Waiting all threads... - done", PR_VERY_LOW);
    return status;
}
