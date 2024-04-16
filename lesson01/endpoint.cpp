#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
//#include <boost/system/detail/error_code.hpp>
#include <cstddef>
#include <cstdio>
#include <fmt/core.h>
#include <memory>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <string>
using namespace boost;
int client_endpoint() {
    std::string raw_ip_address = "127.0.0.1";
    system::error_code ec;
    asio::ip::address ip_address =
        asio::ip::address::from_string(raw_ip_address, ec);
    if (ec.value() != 0) {
        fmt::print(
            "Failed to parse the IP address. Error code: {}. Message: {}.",
            ec.value(), ec.message());
        return ec.value();
    }
    return 0;
}

int server_endpoint() {
    unsigned short port = 2080;
    asio::ip::address ip_address = asio::ip::address_v4::any();
    asio::ip::tcp::endpoint ep(ip_address, port);
    return 0;
}

int create_tcp_socket() {
    asio::io_context ioc;
    asio::ip::tcp::socket sock(ioc);
    asio::ip::tcp protocol = asio::ip::tcp::v4();
    boost::system::error_code ec;
    sock.open(protocol, ec);
    if (ec.value() != 0) {
        fmt::print("Failed to open the socket. Error code: {}. Message: {}.",
                     ec.value(), ec.message());
        return ec.value();
    }
    return 0;
}

int create_acceptor_socket() {
    asio::io_context ioc;
    asio::ip::tcp::acceptor acceptor(
        ioc, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 6666));
    return 0;
}

int bind_acceptor_socket() {
    unsigned short port = 6666;
    return port;
}

void use_buffer_str() {
    asio::const_buffers_1 output = asio::buffer("hello world");
    std::unique_ptr<char[]> buf(new char[30]);
    auto input_buffer = asio::buffer(buf.get(), 30);
}

void write_to_socket(asio::ip::tcp::socket& sock) {
    std::string data = "hello world";
    std::size_t total_bytes_transferred = 0;
    while (total_bytes_transferred != data.length()) {
        total_bytes_transferred += sock.write_some(
            asio::buffer(data.c_str() + total_bytes_transferred,
                         data.length() - total_bytes_transferred));
    }
}

int send_data_by_write_some() {
    std::string raw_ip_address = "192.168.3.11";
    unsigned short port_num = 3333;
    try {
        asio::ip::tcp::endpoint ep(
            asio::ip::address::from_string(raw_ip_address), port_num);
        asio::io_context ioc;
        asio::ip::tcp::socket sock(ioc, ep.protocol());
        sock.connect(ep);
        std::string buf = "hello world";
        int send_length =
            asio::write(sock, asio::buffer(buf.c_str(), buf.length()));
        if (send_length <= 0) {
            return -1;
        }
    } catch (std::system_error& e) {
        fmt::print("Error occurred! Error code: {}. Message: {}.",
                     e.code().value(), e.what());
        return e.code().value();
    }
    return 0;
}

std::string read_from_socket(asio::ip::tcp::socket& sock) {
    constexpr unsigned short MESSAGE_SIZE = 7;
    char buf[MESSAGE_SIZE];
    std::size_t total_bytes_read = 0;
    while (total_bytes_read != MESSAGE_SIZE) {
        total_bytes_read += sock.read_some(asio::buffer(
            buf + total_bytes_read, MESSAGE_SIZE - total_bytes_read));
    }

    return std::string(buf, total_bytes_read);
}

int read_data_by_read_some() {
    std::string raw_ip_address = "127.0.0.1";
    unsigned short port_num = 3333;
    try {
        asio::ip::tcp::endpoint ep(
            asio::ip::address::from_string(raw_ip_address), port_num);
        asio::io_context ioc;
        asio::ip::tcp::socket sock(ioc, ep.protocol());
        sock.connect(ep);
        auto data = read_from_socket(sock);
        for (auto i : std::vector<int>()) {
        }
    } catch (std::system_error& e) {
        fmt::print("Error occurred! Error code: {}. Message: {}.",
                     e.code().value(), e.what());
        return e.code().value();
    }
    return 0;
}

int main() {
    client_endpoint();
    spdlog::set_level(spdlog::level::debug);
    spdlog::debug("Hello, {}!", "World");
}