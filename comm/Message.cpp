//
// Created by stevezbiz on 26/11/20.
//

#include "Message.h"

template<class Archive>
void Message::serialize(Archive &ar, const unsigned int version) {
    ar & this->code;
    ar & this->username;
    ar & this->password;
    ar & this->path;
    ar & this->hash;
    ar & this->paths;
    ar & this->hashes;
    ar & this->file_string_;
    ar & this->elementStatus;
    ar & this->comm_error;
    ar & this->status; // = okay
    ar & this->cookie;
    ar & this->keep_alive;
}

std::vector<boost::asio::const_buffer> Message::send() {
    std::ostringstream header_stream{};
    std::ostringstream content_stream{};
    boost::archive::text_oarchive oa{ content_stream };

    // prepare data
    std::ostringstream file_stream;
    for(unsigned char c: this->file) {
        file_stream << c;
    }
    std::string file_string{ file_stream.str() };
    this->file_string_ = file_string;

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

std::shared_ptr<Message> Message::okay() {
    return std::make_shared<Message>(MSG_OKAY);
}

std::shared_ptr<Message> Message::error(Comm_error *comm_error) {
    auto message = std::make_shared<Message>(MSG_ERROR);
    message->comm_error = std::shared_ptr<Comm_error>(comm_error);
    return message;
}

std::shared_ptr<Message> Message::login(const std::string &username, const std::string &password) {
    auto message = std::make_shared<Message>(MSG_LOGIN);
    message->username = username;
    message->password = password;
    return message;
}

std::shared_ptr<Message> Message::probe(const std::vector<std::string> &paths) {
    auto message = std::make_shared<Message>(MSG_PROBE);
    message->paths = paths;
    return message;
}

std::shared_ptr<Message> Message::probe_content(const std::unordered_map<std::string, std::string> &hashes) {
    auto message = std::make_shared<Message>(MSG_PROBE_CONTENT);
    message->hashes = hashes;
    return message;
}

std::shared_ptr<Message> Message::get(const std::string &path) {
    auto message = std::make_shared<Message>(MSG_GET);
    message->path = path;
    return message;
}

std::shared_ptr<Message> Message::get_content(const std::vector<unsigned char> &file, const std::string &path) {
    auto message = std::make_shared<Message>(MSG_GET_CONTENT);
    message->file = file;
    message->path = path;
    return message;
}

std::shared_ptr<Message> Message::push(const std::vector<unsigned char> &file, const std::string &path, const std::string &hash, ElementStatus elementStatus) {
    auto message = std::make_shared<Message>(MSG_PUSH);
    message->file = file;
    message->path = path;
    message->hash = hash;
    message->elementStatus = elementStatus;
    return message;
}

std::shared_ptr<Message> Message::restore() {
    return std::make_shared<Message>(MSG_RESTORE);
}

std::shared_ptr<Message> Message::restore_content(const std::vector<std::string> &paths) {
    auto message = std::make_shared<Message>(MSG_RESTORE_CONTENT);
    message->paths = paths;
    return message;
}

std::shared_ptr<Message> Message::end() {
    return std::make_shared<Message>(MSG_END);
}

std::shared_ptr<Message> Message::build_header() {
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
    Logger::info("Message::build_header", "Content code " + std::to_string(msg_code), PR_VERY_LOW);
    Logger::info("Message::build_header", "Content length " + std::to_string(content_length), PR_VERY_LOW);
    Logger::info("Message::build_header", "Building message header... - done", PR_LOW);

    return shared_from_this();
}

std::shared_ptr<Message> Message::build_content() const {
    Logger::info("Message::build_content", "Building message content...", PR_LOW);
    Logger::info("Message::build_header", "Header buffer raw content: \"" + std::string{ this->content_buffer_, this->content_buffer_length_ } + "\"", PR_LOW);
    std::string archive_data{ this->content_buffer_, this->content_buffer_length_ };
    std::istringstream archive_stream{ archive_data };
    boost::archive::text_iarchive ia{ archive_stream };
    Message *new_message;
    ia >> new_message; // de-serialization
    new_message->file.clear();
    std::vector<unsigned char>file_vet{ new_message->file_string_.begin(), new_message->file_string_.end() };
    new_message->file.clear();
    new_message->file = file_vet;
    new_message->file_string_.clear();
    Logger::info("Message::build_content", "Building message content... - done", PR_LOW);

    return std::shared_ptr<Message>(new_message);
}

[[nodiscard]] boost::asio::mutable_buffer Message::get_header_buffer() const { // generic pointer, sorry
    return boost::asio::buffer(this->header_buffer_, code_length + length_length);
}

[[nodiscard]] boost::asio::mutable_buffer Message::get_content_buffer() const { // generic pointer, sorry
    return boost::asio::buffer(this->content_buffer_, this->content_buffer_length_);
}

Message::~Message() {
    delete[] this->header_buffer_;
    delete[] this->content_buffer_;
}
