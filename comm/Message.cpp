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
        content_buffer_(nullptr) {}

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

std::vector<boost::asio::const_buffer> Message::send() {
    std::ostringstream header_stream{};
    std::ostringstream content_stream{};
    boost::archive::text_oarchive oa{ content_stream };

    // serialize message content
    Logger::info("Message::send", "Serializing new message...", PR_LOW);
    oa << this;
    this->content_data_ = content_stream.str();

    // fixed lengths for the "type" and "length" values
    int msg_code = this->code;
    header_stream << std::setw(code_length) << msg_code;
    header_stream << std::setw(length_length) << std::hex << content_data_.length();
    this->header_data_ = header_stream.str();
    Logger::info("Message::send", "Header " + this->header_data_, PR_VERY_LOW);
    Logger::info("Message::send", "Content " + this->content_data_, PR_VERY_LOW);

    // concat buffers into a vector
    std::vector<boost::asio::const_buffer> out_buffers{};
    out_buffers.reserve(2);
    out_buffers.emplace_back(boost::asio::buffer(this->header_data_, code_length + length_length));
    out_buffers.emplace_back(boost::asio::buffer(this->content_data_, this->content_data_.length()));
    Logger::info("Message::send", "Serializing new message... - done, code " + std::to_string(this->code) + " length " + std::to_string(this->content_data_.size()), PR_LOW);

    return out_buffers;
}

[[nodiscard]] bool Message::is_okay() const { return this->status; }

Message::Message(MESSAGE_TYPE code) :
        code(code),
        status(true),
//        header_buffer(new struct_header_buffer{MSG_ERROR, 0 }),
        content_buffer_(nullptr),
        comm_error(nullptr) {
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
    Logger::info("Message::build_header", "Building message header...", PR_LOW);
    Logger::info("Message::build_header", "Header buffer raw content: \"" + std::string{this->header_buffer_, code_length + length_length } + "\"", PR_LOW);
    // auto buffer = reinterpret_cast<char *>(this->header_buffer);
    std::istringstream is{ std::string{this->header_buffer_, code_length + length_length }};
    int msg_code = MSG_ERROR;
    std::size_t content_length = 0;
    if(!(is >> msg_code))
        throw std::invalid_argument( "Cannot read content code");
    if(!(is >> std::hex >> content_length))
        throw std::invalid_argument( "Cannot read content length");
    this->code = static_cast<MESSAGE_TYPE>(msg_code);
    this->content_buffer_length_ = content_length;
    this->content_buffer_ = new char[content_length]();
//    this->content_buffer->resize(content_length);

    /*
    this->code = this->header_buffer->type;
    this->content_buffer->resize(this->header_buffer->length);
     */
    /*
    delete this->content_buffer;
    this->content_buffer = new char[this->header_buffer->length]; // prepare the content buffer
    delete this->header_buffer; // not necessary anymore
    */
    Logger::info("Message::build_header", "Content code " + std::to_string(msg_code), PR_VERY_LOW);
    Logger::info("Message::build_header", "Content length " + std::to_string(content_length), PR_VERY_LOW);
    Logger::info("Message::build_header", "Building message header... - done", PR_LOW);

    return this;
}

Message *Message::build_content() const {
    Logger::info("Message::build_content", "Building message content...", PR_LOW);
//    auto vet = reinterpret_cast<char *>(&this->content_buffer[0]);
    std::string archive_data{ this->content_buffer_, this->content_buffer_length_ };
    std::istringstream archive_stream{ archive_data };
    // std::istringstream ss{ std::string{ this->content_buffer, this->header_buffer->length }};
    boost::archive::text_iarchive ia{ archive_stream };
    Message *new_message;
    ia >> new_message; // de-serialization
    // delete this->content_buffer; // not necessary anymore
    Logger::info("Message::build_content", "Building message content... - done", PR_LOW);

    return new_message;
}

[[nodiscard]] boost::asio::mutable_buffer Message::get_header_buffer() const { // generic pointer, sorry
    return boost::asio::buffer(this->header_buffer_, code_length + length_length);
}

[[nodiscard]] boost::asio::mutable_buffer Message::get_content_buffer() const { // generic pointer, sorry
    return boost::asio::buffer(this->content_buffer_, this->content_buffer_length_);
}

Message::~Message() {
    delete this->comm_error;
    delete[] this->header_buffer_;
    delete[] this->content_buffer_;
}
