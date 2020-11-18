//
// Created by stevezbiz on 07/11/20.
//

#include "Client.h"
#include <utility>

using namespace boost::asio;
/**
 * Inizializza i campi dell'oggetto e lancia la procedura di connessione al server
 * @param ctx: i servizi di I/O forniti
 * @param endpoint_iterator: l'indirizzo risolto del server a cui connettersi
 */
Client::Client(io_context &ctx, ip::tcp::resolver::iterator endpoint_iterator)
        : ctx_(ctx), socket_(ctx) {

    m_path = "/home/carlo/CLion/CLionProjects/Progetto/filetransfer/test.txt";

    do_connect(std::move(endpoint_iterator));

    openFile(m_path);
}

/**
 * Invia un messaggio al server
 * @param msg: il messaggio da inviare al server
 */
void Client::write(const Message &msg) {
    ctx_.post([this, msg]() {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress) {
            do_write();
        }
    });
}

/**
 * Chiude la connessione con il server
 */
void Client::close() {
    ctx_.post([this]() { socket_.close(); });
}

/**
 * Si connette al server e pone il client in ascolto di comunicazioni provenienti dal server
 * @param endpoint_iterator: l'indirizzo risolto del server a cui connettersi
 */
void Client::do_connect(ip::tcp::resolver::iterator endpoint_iterator) {
    async_connect(socket_, std::move(endpoint_iterator),
                  [this](boost::system::error_code ec, const ip::tcp::resolver::iterator &it) {
                      if (!ec) {
                          if (m_path != "");
                            writeBuffer(m_request);
                          do_read_header();
                      } else {
                          std::cout << "Coudn't connect to host. Please run server "
                                       "or check network connection.\n";
                      }
                  });
}

/**
 * Legge l'header del messaggio in arrivo, lo decodifica e lancia la lettura del body
 */
void Client::do_read_header() {
    async_read(socket_, buffer(read_msg_.getData(), Message::header_length),
               [this](boost::system::error_code ec, std::size_t length) {
                   if (!ec && read_msg_.decodeHeader()) {
                       do_read_body();
                   } else {
                       socket_.close();
                   }
               });
}

/**
 * Legge il body sulla base delle informazioni presenti nell'header e si pone in ascolto del prossimo header
 */
void Client::do_read_body() {
    async_read(socket_, buffer(read_msg_.getBody(), read_msg_.getBodyLength()),
               [this](boost::system::error_code ec, std::size_t length) {
                   if (!ec) {
                       std::cout.write(read_msg_.getBody(), read_msg_.getBodyLength());
                       std::cout << "\n";
                       do_read_header();
                   } else {
                       socket_.close();
                   }
               });
}

/**
 * Esegue l'invio di un messaggio al server
 */
void Client::do_write() {
    async_write(socket_, buffer(write_msgs_.front().getData(), write_msgs_.front().getLength()),
                [this](boost::system::error_code ec, std::size_t length) {
                    if (!ec) {
                        write_msgs_.pop_front();
                        if (!write_msgs_.empty()) {
                            do_write();
                        }
                    } else {
                        socket_.close();
                    }
                });
}
/**
 * Open a file from a path and initialize asio::streambuf and ifstream
 */
void Client::openFile(std::string const& t_path)
{
    m_sourceFile.open(t_path, std::ios_base::binary | std::ios_base::ate);
    if (m_sourceFile.fail()) {
        throw std::fstream::failure("Failed while opening file " + t_path + "\nTry to insert complete and correct path");
    }

    m_sourceFile.seekg(0, m_sourceFile.end);
    auto fileSize = m_sourceFile.tellg();
    m_sourceFile.seekg(0, m_sourceFile.beg);

    std::ostream requestStream(&m_request);
    boost::filesystem::path p(t_path);
    //insert on the top of the stream filename and filesize
    requestStream << p.filename().string() << "\n" << fileSize << "\n\n";
    std::cout << "FILENAME: "<< p.filename().string() << "\nFILESIZE: "<< fileSize<< std::endl;
}
/**
 * Actural write in the socket using
 */

void Client::doWriteFile(const boost::system::error_code& t_ec)
{
    if (!t_ec) {
        if (m_sourceFile) {
            m_sourceFile.read(m_buf.data(), m_buf.size());
            if (m_sourceFile.fail() && !m_sourceFile.eof()) {
                auto msg = "Failed while reading file";
                throw std::fstream::failure(msg);
            }
            std::stringstream ss;
            ss << "...sending " << m_sourceFile.gcount() << " bytes";

            std::cout << ss.str() << std::endl;

            auto buf = boost::asio::buffer(m_buf.data(), static_cast<size_t>(m_sourceFile.gcount()));
            writeBuffer(buf);
        }
    } else {
        std::cerr << "error writing in the socket" << std::endl;
    }
}
