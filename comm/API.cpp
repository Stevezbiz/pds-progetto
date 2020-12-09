//
// Created by stevezbiz on 26/11/20.
//

#include "API.h"

template<typename T>
API<T>::API(T *socket_api) : api_(socket_api) {}

template<typename T>
bool API<T>::save_file_(Message *message) {
    Utils::write_on_file(fs::path{ message->path }, message->file);
    return true;
}

template<typename T>
Message *API<T>::get_message() {
    return this->api_->get_message();
}

template<typename T>
Comm_error *API<T>::get_last_error() {
    return this->api_->get_last_error();
}

template<typename T>
API<T>::~API() {
    delete this->api_;
}

//API::API(Socket_API *socket_api) : api_(socket_api) {}
//
//bool API::save_file_(Message *message) {
//    Utils::write_on_file(fs::path{ message->path }, message->file);
//    return true;
//}
//
//Message *API::get_message() {
//    return this->api_->get_message();
//}
//
//Comm_error *API::get_last_error() {
//    return this->api_->get_last_error();
//}
//
//API::~API() {
//    delete this->api_;
//}
