#pragma once

#include "MsgNode.h"
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
//#include <boost/system/detail/error_code.hpp>
#include <cstddef>
#include <memory>
#include <mutex>
#include <queue>
using namespace boost;

class Server;
class Session : public std::enable_shared_from_this<Session> {
public:
    Session(asio::io_context& ioc, Server* server);
    ~Session();

    asio::ip::tcp::socket& get_socket();
    std::string get_uuid();

    void start();
    void send(const char* msg, int max_length,short msg_id);
    void send(const std::string& msg,short msg_id);
    void close();

private:
    void handle_read(const system::error_code& error,
                     std::size_t bytes_transferred,
                     std::shared_ptr<Session> _self_shared);
    void handle_write(const system::error_code& error,
                      std::shared_ptr<Session> _self_shared);

    asio::ip::tcp::socket _socket;
    std::string _uuid;
    Server* _server;
    static constexpr int MAX_LENGTH = 1024;
    char _data[MAX_LENGTH];
    std::queue<std::shared_ptr<SendNode>> _send_que;
    bool _b_head_parse;
    std::shared_ptr<RecvNode> _recv_msg_node;
    std::shared_ptr<MsgNode> _recv_head_node;
    std::mutex _send_lock;
};