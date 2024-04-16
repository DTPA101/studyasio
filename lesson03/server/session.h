#pragma once

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
//#include <boost/system/detail/error_code.hpp>
#include <cstddef>
using namespace boost;

class Session {
public:
    Session(asio::io_context& ioc);
    asio::ip::tcp::socket& get_socket();

    void start();

private:
    void handle_read(const system::error_code& error,
                     std::size_t bytes_transferred);
    void handle_write(const system::error_code& error);

    asio::ip::tcp::socket _socket;
    static constexpr int MAX_LENGTH = 1024;
    char _data[MAX_LENGTH];
};