#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/write.hpp>
//#include <boost/system/detail/error_code.hpp>
#include <cstddef>
#include <cstring>
#include <exception>
#include <fmt/core.h>
#include <memory>
#include <set>
#include <spdlog/spdlog.h>
#include <system_error>
#include <thread>
using namespace boost;
using socket_ptr = std::shared_ptr<asio::ip::tcp::socket>;
constexpr int MAX_LENGTH = 1024;
std::set<std::shared_ptr<std::thread>> thread_set;

void session(socket_ptr sock) {
    try {
        for (;;) {
            char data[MAX_LENGTH];
            std::memset(data, 0, MAX_LENGTH);
            system::error_code ec;
            std::size_t length = sock->read_some(
                asio::buffer(data, MAX_LENGTH), ec);
            if (ec == asio::error::eof) {
                spdlog::info("connect closed by peer");
                break;
            } else if (ec) {
                throw std::system_error(ec);
            }
            fmt::print("receive data from {}",
                         sock->remote_endpoint()
                             .address()
                             .to_string());
            fmt::print("data: {}", data);
            asio::write(*sock, asio::buffer(data, length));
        }
    } catch (const std::exception& e) {
        spdlog::error("exception in session: {}", e.what());
    }
}

void server(asio::io_context& ioc, unsigned short port) {
    asio::ip::tcp::acceptor acceptor(
        ioc,
        asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
    spdlog::info("server start listening at port {}", port);
    for (;;) {
        socket_ptr socket =
            std::make_shared<asio::ip::tcp::socket>(ioc);
        acceptor.accept(*socket);
        auto t =
            std::make_shared<std::thread>(session, socket);
        thread_set.insert(t);
    }
}

int main() {
    spdlog::set_level(spdlog::level::debug);
    try {
        asio::io_context ioc;
        server(ioc, 6666);
        for (auto& t : thread_set) {
            t->join();
        }
    } catch (const std::exception& e) {
        spdlog::error("exception in thread: {}", e.what());
    }
}