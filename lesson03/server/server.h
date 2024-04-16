#pragma once
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
//#include <boost/system/detail/error_code.hpp>
using namespace boost;
class Session;
class Server {
public:
    Server(asio::io_context& ioc, unsigned short port);

private:
    void start_accept();
    void handle_accept(Session* new_session, const system::error_code& error);

    asio::io_context& _ioc;
    asio::ip::tcp::acceptor _acceptor;
};