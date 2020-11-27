//
// Created by stevezbiz on 26/11/20.
//

#include "Message.h"

std::vector <boost::asio::const_buffer> Message::send() {
    std::ostringstream type_stream{};
    std::ostringstream length_stream{};
    std::ostringstream value_stream{};
    boost::archive::text_oarchive oa{value_stream};

    // serialize message content
    oa << this;
    auto value_data = value_stream.str();

    // fixed lengths for the "type" and "length" values
    type_stream << std::setw(type_length) << std::hex << this->code;
    length_stream << std::setw(length_length) << std::hex << value_data.length();
    auto type_data = value_stream.str();
    auto length_data = value_stream.str();

    // concat buffers into a vector
    std::vector <boost::asio::const_buffer> out_buffers;
    out_buffers.emplace_back(boost::asio::buffer(type_data));
    out_buffers.emplace_back(boost::asio::buffer(length_data));
    out_buffers.emplace_back(boost::asio::buffer(value_data));

    return out_buffers;
}

[[nodiscard]] bool Message::is_okay() const { return this->status; }

Message::Message(MESSAGE_TYPE code) : code(code), status(true), header_buffer(new struct_header_buffer{ ERROR, 0 }), content_buffer(nullptr) {
    if (code == ERROR)
        status = false;
}

Message *Message::okay() {
    return new Message{ OKAY };
}

Message *Message::error(Comm_error *comm_error) {
    auto message = new Message{ ERROR };
    message->comm_error = comm_error;
    return message;
}

Message *Message::login(const std::string &username, const std::string &password) {
    auto message = new Message{ LOGIN };
    message->username = username;
    message->password = password;
    return message;
}

Message *Message::probe(const std::vector<std::string> &paths) {
    auto message = new Message{ PROBE };
    message->paths = paths;
    return message;
}

Message *Message::probe_content(const std::map<std::string, std::string> &hashes) {
    auto message = new Message{ PROBE_CONTENT };
    message->hashes = hashes;
    return message;
}

Message *Message::get(const std::string &path) {
    auto message = new Message{ GET };
    message->path = path;
    return message;
}

Message *Message::get_content(const std::vector<unsigned char> &file, const std::string &path) {
    auto message = new Message{ GET_CONTENT };
    message->file = file;
    message->path = path;
    return message;
}

Message *Message::push(const std::vector<unsigned char> &file, const std::string &path, const std::string &hash) {
    auto message = new Message{ PUSH };
    message->file = file;
    message->path = path;
    message->hash = hash;
    return message;
}

Message *Message::restore() {
    auto message = new Message{ RESTORE };
    return message;
}

Message *Message::restore_content(const std::vector<std::string> &paths) {
    auto message = new Message{ RESTORE_CONTENT };
    message->paths = paths;
    return message;
}

Message *Message::end() {
    return new Message{ END };
}

Message *Message::build() {
    if (this->content_buffer == nullptr) { // if only the header has been received
        this->code = this->header_buffer->type;
        this->content_buffer = new char[this->header_buffer->length]; // prepare the content buffer
        return this;
    }

    // else if the content as been received
    std::stringstream ss{ this->content_buffer };
    boost::archive::text_iarchive ia{ ss };
    auto *new_message = new Message{};
    ia >> *new_message; // de-serialization
    return new_message;
}

[[nodiscard]] boost::asio::mutable_buffer Message::get_header_buffer() const { // generic pointer, sorry
    return boost::asio::mutable_buffer(this->header_buffer, sizeof(struct_header_buffer));
}

[[nodiscard]] boost::asio::mutable_buffer Message::get_content_buffer() const { // generic pointer, sorry
    return boost::asio::mutable_buffer(this->content_buffer, sizeof(this->header_buffer->length)));
}