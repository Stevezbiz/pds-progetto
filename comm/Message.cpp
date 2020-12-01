//
// Created by stevezbiz on 26/11/20.
//

#include "Message.h"

Message::Message(MESSAGE_TYPE code, std::string username, std::string password, std::string path, std::string hash,
                 std::vector<std::string> paths, std::map<std::string, std::string> hashes,
                 std::vector<unsigned char> file, ElementStatus elementStatus, Comm_error *comm_error, bool status) :
        code(code),
        username(std::move(username)),
        password(std::move(password)),
        path(std::move(path)),
        hash(std::move(hash)),
        paths(std::move(paths)),
        hashes(std::move(hashes)),
        file(std::move(file)),
        elementStatus(elementStatus),
        comm_error(comm_error),
        status(status),
        header_buffer(new struct_header_buffer{MSG_ERROR, 0 }),
        content_buffer(new std::vector<char>{}) {}

template<class Archive>
void Message::serialize(Archive &ar, const unsigned int version) {
    ar & this->code;
    ar & this->username;
    ar & this->password;
    ar & this->path;
    ar & this->hash;
    ar & this->paths;
    ar & this->hashes;
    ar & this->file;
    ar & this->elementStatus;
    ar & this->comm_error;
    ar & this->status; // = okay
}

std::vector<boost::asio::const_buffer> Message::send() const {
    std::ostringstream type_stream{};
    std::ostringstream length_stream{};
    std::ostringstream value_stream{};
    boost::archive::text_oarchive oa{ value_stream };

    // serialize message content
    oa << this;
    // oa << this->comm_error;
    auto value_data = value_stream.str();

    // fixed lengths for the "type" and "length" values
    type_stream << std::setw(type_length) << std::hex << this->code;
    length_stream << std::setw(length_length) << std::hex << value_data.length();
    auto type_data = type_stream.str();
    auto length_data = length_stream.str();

    // concat buffers into a vector
    std::vector <boost::asio::const_buffer> out_buffers{};
    out_buffers.emplace_back(boost::asio::buffer(type_data));
    out_buffers.emplace_back(boost::asio::buffer(length_data));
    out_buffers.emplace_back(boost::asio::buffer(value_data));
    Logger::info("Message::send", "New message ready to be sent", PR_LOW);

    return out_buffers;
}

[[nodiscard]] bool Message::is_okay() const { return this->status; }

Message::Message(MESSAGE_TYPE code) : code(code), status(true), header_buffer(new struct_header_buffer{MSG_ERROR, 0 }), content_buffer(new std::vector<char>{}), comm_error(nullptr) {
    if(this->code == MSG_ERROR)
        this->status = false;
}

Message *Message::okay() {
    return new Message{MSG_OKAY };
}

Message *Message::error(Comm_error *comm_error) {
    auto message = new Message{MSG_ERROR };
    message->comm_error = comm_error;
    return message;
}

Message *Message::login(const std::string &username, const std::string &password) {
    auto message = new Message{MSG_LOGIN };
    message->username = username;
    message->password = password;
    return message;
}

Message *Message::probe(const std::vector<std::string> &paths) {
    auto message = new Message{MSG_PROBE };
    message->paths = paths;
    return message;
}

Message *Message::probe_content(const std::map<std::string, std::string> &hashes) {
    auto message = new Message{MSG_PROBE_CONTENT };
    message->hashes = hashes;
    return message;
}

Message *Message::get(const std::string &path) {
    auto message = new Message{MSG_GET };
    message->path = path;
    return message;
}

Message *Message::get_content(const std::vector<unsigned char> &file, const std::string &path) {
    auto message = new Message{MSG_GET_CONTENT };
    message->file = file;
    message->path = path;
    return message;
}

Message *Message::push(const std::vector<unsigned char> &file, const std::string &path, const std::string &hash, ElementStatus elementStatus) {
    auto message = new Message{MSG_PUSH };
    message->file = file;
    message->path = path;
    message->hash = hash;
    message->elementStatus = elementStatus;
    return message;
}

Message *Message::restore() {
    auto message = new Message{MSG_RESTORE };
    return message;
}

Message *Message::restore_content(const std::vector<std::string> &paths) {
    auto message = new Message{MSG_RESTORE_CONTENT };
    message->paths = paths;
    return message;
}

Message *Message::end() {
    return new Message{MSG_END };
}

Message *Message::build_header() {
    this->code = this->header_buffer->type;
    /*
    delete this->content_buffer;
    this->content_buffer = new char[this->header_buffer->length]; // prepare the content buffer
    delete this->header_buffer; // not necessary anymore
    */
    this->content_buffer->resize(this->header_buffer->length);
    Logger::info("Message::build_header", "New message header has been built correctly, code " + std::to_string(this->code) + " length " + std::to_string(this->header_buffer->length), PR_LOW);

    return this;
}

Message *Message::build_content() const {
    auto vet = *this->content_buffer;
    std::string archive_data{ &vet[0], this->content_buffer->size() };
    std::istringstream archive_stream{ archive_data };
    // std::istringstream ss{ std::string{ this->content_buffer, this->header_buffer->length }};
    boost::archive::text_iarchive ia{ archive_stream };
    Message *new_message;
    ia >> new_message; // de-serialization
    // delete this->content_buffer; // not necessary anymore
    Logger::info("Message::build_content", "New message content has been built correctly, code " + std::to_string(new_message->code), PR_LOW);

    return new_message;
}

[[nodiscard]] boost::asio::mutable_buffer Message::get_header_buffer() const { // generic pointer, sorry
    return boost::asio::mutable_buffer(this->header_buffer, sizeof(struct_header_buffer));
}

[[nodiscard]] boost::asio::mutable_buffer Message::get_content_buffer() const { // generic pointer, sorry
    return boost::asio::mutable_buffer(this->content_buffer, this->header_buffer->length);
}

Message::~Message() {
    delete this->comm_error;
    delete this->header_buffer;
    delete this->content_buffer;
}
