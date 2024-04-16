#pragma once
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
//#include <boost/system/detail/error_code.hpp>
#include <map>
using namespace boost;

class Session;
class Server {
public:
    Server(asio::io_context& ioc, unsigned short port);

    void clear_session(const std::string& uuid);

private:
    void start_accept();
    void handle_accept(std::shared_ptr<Session> new_session,
                       const system::error_code& error);

    asio::io_context& _ioc;
    asio::ip::tcp::acceptor _acceptor;
    std::map<std::string, std::shared_ptr<Session>>
        _sessions;
};