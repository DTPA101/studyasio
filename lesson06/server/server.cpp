#include "server.h"
#include "session.h"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <utility>
#include "AsioIOContextPool.h"

Server::Server(asio::io_context& ioc, unsigned short port)
    : _ioc(ioc),
      _acceptor(ioc, asio::ip::tcp::endpoint(
                         asio::ip::tcp::v4(), port)) {
    start_accept();
    spdlog::info("server start at port {}", port);
}

void Server::clear_session(const std::string& uuid) {
    std::lock_guard<std::mutex>lock (_mutex);
    _sessions.erase(uuid);
}

void Server::start_accept() {
    auto& io_context=AsioIOContextPool::instance()->get_io_context();
    auto new_session =
        std::make_shared<Session>(_ioc, this);
    _acceptor.async_accept(
        new_session->get_socket(),
        std::bind(&Server::handle_accept, this, new_session,
                  std::placeholders::_1));
}

void Server::handle_accept(
    std::shared_ptr<Session> new_session,
    const system::error_code& error) {
    if (!error) {
        new_session->start();
        _sessions.insert(std::make_pair(
            new_session->get_uuid(), new_session));
    }
    start_accept();
}