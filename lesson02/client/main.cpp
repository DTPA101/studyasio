#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
//#include <boost/system/detail/error_code.hpp>
#include <cstddef>
#include <cstring>
#include <exception>
#include <fmt/core.h>
#include <iostream>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
constexpr int MAX_LENGTH = 1024;
using namespace boost;
int main() {
    spdlog::set_level(spdlog::level::debug);
    try {
        asio::io_context ioc;
        asio::ip::tcp::endpoint remote_ep(
            asio::ip::address::from_string("127.0.0.1"),
            9999);
        system::error_code ec = asio::error::host_not_found;
        asio::ip::tcp::socket sock(ioc);
        sock.connect(remote_ep, ec);
        if (ec) {
            spdlog::error(
                "connect failed, code: {}, error msg: {}",
                ec.value(), ec.message());
            return -1;
        }
        fmt::print("enter message: ");
        char request[MAX_LENGTH];
        std::cin.getline(request, MAX_LENGTH);
        std::size_t request_length = strlen(request);
        asio::write(sock,
                    asio::buffer(request, request_length));

        char reply[MAX_LENGTH];
        std::size_t reply_length = asio::read(
            sock, asio::buffer(reply, request_length));
        fmt::print("reply: {}", reply);
    } catch (const std::exception& e) {
        spdlog::error("exception: {}", e.what());
    }
}