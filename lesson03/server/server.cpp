#include "server.h"
#include "session.h"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <spdlog/spdlog.h>

Server::Server(asio::io_context& ioc, unsigned short port)
    : _ioc(ioc),
      _acceptor(ioc, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {
    start_accept();
    spdlog::info("server start at port {}", port);
}

void Server::start_accept() {
    Session* new_session = new Session(_ioc);
    _acceptor.async_accept(new_session->get_socket(),
                           std::bind(&Server::handle_accept, this, new_session,
                                     std::placeholders::_1));
}

void Server::handle_accept(Session* new_session,
                           const system::error_code& error) {
    if (error) {
        delete new_session;
    } else {
        new_session->start();
    }
    start_accept();
}