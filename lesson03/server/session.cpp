#include "session.h"
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
//#include <boost/system/detail/error_code.hpp>
#include <cstring>
#include <functional>
#include <spdlog/spdlog.h>

Session::Session(asio::io_context& ioc) : _socket(ioc) {}

asio::ip::tcp::socket& Session::get_socket() {
    return _socket;
}

void Session::start() {
    std::memset(_data, 0, MAX_LENGTH);
    _socket.async_read_some(
        asio::buffer(_data, MAX_LENGTH),
        std::bind(&Session::handle_read, this,
                  std::placeholders::_1,
                  std::placeholders::_2));
}

void Session::handle_read(const system::error_code& error,
                          std::size_t bytes_transferred) {
    if (error) {
        spdlog::error("read error, code: {}, message: {}",
                      error.value(), error.message());
        delete this;
        return;
    }

    spdlog::info("server receive data: {}", _data);
    asio::async_write(
        _socket, asio::buffer(_data, bytes_transferred),
        std::bind(&Session::handle_write, this,
                  std::placeholders::_1));
}

void Session::handle_write(
    const system::error_code& error) {
    if (error) {
        spdlog::error("write error, code: {}, message: {}",
                      error.value(), error.message());
        delete this;
        return;
    }

    std::memset(_data, 0, MAX_LENGTH);
    _socket.async_read_some(
        asio::buffer(_data, MAX_LENGTH),
        std::bind(&Session::handle_read, this,
                  std::placeholders::_1,
                  std::placeholders::_2));
}