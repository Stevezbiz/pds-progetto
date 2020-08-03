/**
 * Gastaldi Paolo
 * 03-08-2020
 *
 * test 00
 */

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main() {
    boost::asio::io_context io_context{1};
    tcp::resolver resolver{io_context};

    return 0;
}
