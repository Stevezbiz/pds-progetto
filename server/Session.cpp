//
// Created by stevezbiz on 07/11/20.
//

#include <iostream>
#include "Session.h"

using namespace boost::asio;

/**
 * Inizializza i campi dell'oggetto
 * @param socket: socket su cui aprire la connessione
 */
Session::Session(ip::tcp::socket socket) : socket_(std::move(socket)) {}

/**
 * Pone il server in ascolto di messaggi in arrivo dal client
 */
void Session::start() {
    //
    //do_read_header();

    do_read_file();// -> read a file
}

/**
 * Invia un messaggio al client
 * @param msg: messaggio da inviare
 */
void Session::write(const Message &msg) {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress) {
        do_write();
    }
}

/**
 * Legge l'header del messaggio in arrivo, lo decodifica e lancia la lettura del body
 */
void Session::do_read_header() {
    auto self(shared_from_this());
    async_read(socket_, buffer(read_msg_.getData(), Message::header_length),
               [this, self](boost::system::error_code ec, std::size_t length) {
                   if (!ec && read_msg_.decodeHeader()) {
                       do_read_body();
                   } else {

                   }
               });
}

/**
 * Legge il body sulla base delle informazioni presenti nell'header e si pone in ascolto del prossimo header
 */
void Session::do_read_body() {
    auto self(shared_from_this());
    async_read(socket_, buffer(read_msg_.getBody(), read_msg_.getBodyLength()),
               [this, self](boost::system::error_code ec, std::size_t length) {
                   if (!ec) {
                       std::cout << read_msg_.getBody() << std::endl;
                       //write(read_msg_);
                       do_read_header();
                   } else {

                   }
               });
}

/**
 * Esegue l'invio di un messaggio al client
 */
void Session::do_write() {
    auto self(shared_from_this());
    async_write(socket_, buffer(write_msgs_.front().getData(), write_msgs_.front().getLength()),
                [this, self](boost::system::error_code ec, std::size_t length) {
                    if (!ec) {
                        write_msgs_.pop_front();
                        if (!write_msgs_.empty()) {
                            do_write();
                        }
                    } else {

                    }
                });
}
/**
 * Legge il file in arrivo
 */
void Session::do_read_file()
{
    auto self = shared_from_this();
    async_read_until(socket_, m_requestBuf_, "\n\n",
                     [this, self](boost::system::error_code ec, size_t bytes)
                     {
                         if (!ec)
                             process_read(bytes);
                     });
}
/**
 * Processa la lettura
 */
void Session::process_read(size_t t_bytesTransferred)
{
    std::istream requestStream(&m_requestBuf_);
    //read first the file name and file size
    do_read_file_header(requestStream);

    auto pos = m_fileName.find_last_of('\\');
    if (pos != std::string::npos)
        m_fileName = m_fileName.substr(pos + 1);

    create_file();

    auto self = shared_from_this();
    socket_.async_read_some(boost::asio::buffer(m_buf.data(), m_buf.size()),
                             [this, self](boost::system::error_code ec, size_t bytes)
                             {
                                 if (!ec)
                                     do_read_file_body(bytes);
                             });
    std::cout << "Transfer completed" <<std::endl;

}

void Session::do_read_file_header(std::istream &stream)
{

    stream >> m_fileName;
    stream >> m_fileSize;
    stream.read(m_buf.data(), 2);
    std::cout << "Name File insert into stream: "<<m_fileName <<  "\nFile size: " << m_fileSize <<std::endl;
}

void Session::create_file()
{
    m_outputFile.open(m_fileName, std::ios_base::binary);
    if (!m_outputFile) {
        std::cerr << "ERROR CREATING NEW FILE"<< "\n";
        return;
    }

}

void Session::do_read_file_body(size_t t_bytesTransferred)
{
    if (t_bytesTransferred > 0) {
        m_outputFile.write(m_buf.data(), static_cast<std::streamsize>(t_bytesTransferred));
        //std::cout << "What i received: " << m_buf.data() << std::endl;

        if (m_outputFile.tellp() >= static_cast<std::streamsize>(m_fileSize)) {
            //I received the entire file, now is in server m_outputFile (ofstream)
            //termination of the async recursive function
            std::cout << "Received file: " << m_fileName << std::endl;
            return;
        }
    }
    //recursively call the doReadFileContent until the tranfer is completed
    auto self = shared_from_this();
    socket_.async_read_some(boost::asio::buffer(m_buf.data(), m_buf.size()),
                             [this, self](boost::system::error_code ec, size_t bytes)
                             {
                                 do_read_file_body(bytes);
                             });
}
