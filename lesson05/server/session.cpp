#include "session.h"
#include "MsgNode.h"
#include "server.h"
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <cstring>
#include <functional>
#include <memory>
#include <mutex>
#include <spdlog/spdlog.h>
#include "global.h"
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>


Session::Session(asio::io_context& ioc, Server* server)
    : _socket(ioc), _server(server),_b_head_parse(false) {
    auto a_uuid = uuids::random_generator()();
    _uuid = uuids::to_string(a_uuid);
    _recv_head_node = std::make_shared<MsgNode>(HEAD_TOTAL_LEN);
}

asio::ip::tcp::socket& Session::get_socket() {
    return _socket;
}

std::string Session::get_uuid() {
    return _uuid;
}

void Session::start() {
    std::memset(_data, 0, MAX_LENGTH);
    _socket.async_read_some(
        asio::buffer(_data, MAX_LENGTH),
        std::bind(&Session::handle_read, this, std::placeholders::_1,
                  std::placeholders::_2, shared_from_this()));
}

void Session::send(const char* msg, int max_length,short msg_id) {
    std::lock_guard<std::mutex> lock(_send_lock);
    int send_que_size = _send_que.size();
    if (send_que_size > MAX_SEND_QUE) {
        spdlog::warn("session {}: send queue fulled, max size: {}", _uuid,
                     MAX_SEND_QUE);
        return;
    }

    _send_que.push(std::make_shared<SendNode>(msg, max_length,msg_id));

    if (send_que_size > 0) {
        return;
    }

    auto& msgNode = _send_que.front();
    asio::async_write(_socket,
                      asio::buffer(msgNode->_data, msgNode->_total_len),
                      std::bind(&Session::handle_write, this,
                                std::placeholders::_1, shared_from_this()));
}
void Session::send(const std::string &msg,short msg_id){
    send(msg.c_str(),msg.length(),msg_id);
}
void Session::close() {
    _socket.close();
}

Session::~Session() {
    spdlog::debug("{} session destroy}", _uuid);
}

void Session::handle_read(const system::error_code& error,
                          std::size_t bytes_transferred,
                          std::shared_ptr<Session> _self_shared) {
    if (error) {
        spdlog::error("read error, code: {}, message: {}", error.value(),
                      error.message());
        close();
        _server->clear_session(_uuid);
        return;
    }

    int copy_len = 0;
    while (bytes_transferred > 0) {
        if (!_b_head_parse) {
            // 头部没读完且本次读不完头部
            if (bytes_transferred + _recv_head_node->_cur_len < HEAD_TOTAL_LEN) {
                std::memcpy(_recv_head_node->_data + _recv_head_node->_cur_len,
                            _data + copy_len, bytes_transferred);
                std::memset(_data, 0, MAX_LENGTH);
                _recv_head_node->_cur_len += bytes_transferred;
                _socket.async_read_some(asio::buffer(_data, MAX_LENGTH),
                                        std::bind(&Session::handle_read, this,
                                                  std::placeholders::_1,
                                                  std::placeholders::_2,
                                                  _self_shared));
                return;
            }
            // 头部没读完且本次可以读完头部
            int head_remain = HEAD_TOTAL_LEN - _recv_head_node->_cur_len;
            std::memcpy(_recv_head_node->_data + _recv_head_node->_cur_len,
                        _data + copy_len, head_remain);
            copy_len += head_remain;
            bytes_transferred -= head_remain;

            short msg_id=0;
            std::memcpy(&msg_id, _recv_head_node->_data, HEAD_ID_LEN);
            msg_id=asio::detail::socket_ops::network_to_host_short(msg_id);
            if(msg_id>MAX_LENGTH){
                spdlog::error("invalid input",msg_id);
                _server->clear_session(_uuid);
                return;
            }
            // 头部已经读完 获知消息长度
            short data_len=0;
            spdlog::debug("data len: {}", data_len);
            if (data_len > MAX_LENGTH) {
                spdlog::error("invalid data len: {}", data_len);
                _server->clear_session(_uuid);
                return;
            }

            _recv_msg_node = std::make_shared<RecvNode>(data_len,msg_id);
            if (bytes_transferred < data_len) {
                // 消息未读完
                std::memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len,
                            _data + copy_len, bytes_transferred);
                _recv_msg_node->_cur_len += bytes_transferred;
                std::memset(_data, 0, MAX_LENGTH);
                _socket.async_read_some(asio::buffer(_data, MAX_LENGTH),
                                        std::bind(&Session::handle_read, this,
                                                  std::placeholders::_1,
                                                  std::placeholders::_2,
                                                  _self_shared));
                _b_head_parse = true;
                return;
            }

            // 消息全部读完
            std::memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len,
                        _data + copy_len, data_len);
            _recv_msg_node->_cur_len += data_len;
            copy_len += data_len;
            bytes_transferred -= data_len;
            _recv_msg_node->_data[_recv_msg_node->_total_len] = '\0';
            spdlog::info("receive data: {}", _recv_msg_node->_data);

            send(_recv_msg_node->_data, _recv_msg_node->_total_len);
            _b_head_parse = false;
            _recv_head_node->clear();

            if (bytes_transferred <= 0) {
                std::memset(_data, 0, MAX_LENGTH);
                _socket.async_read_some(asio::buffer(_data, MAX_LENGTH),
                                        std::bind(&Session::handle_read, this,
                                                  std::placeholders::_1,
                                                  std::placeholders::_2,
                                                  _self_shared));
                return;
            }
            continue;
        }
        int remain_msg = _recv_msg_node->_total_len - _recv_msg_node->_cur_len;
        if (bytes_transferred < remain_msg) {
            std::memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len,
                        _data + copy_len, bytes_transferred);
            _recv_msg_node->_cur_len += bytes_transferred;
            std::memset(_data, 0, MAX_LENGTH);
            _socket.async_read_some(
                asio::buffer(_data, MAX_LENGTH),
                std::bind(&Session::handle_read, this, std::placeholders::_1,
                          std::placeholders::_2, _self_shared));
            return;
        }

        std::memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len,
                    _data + copy_len, remain_msg);
        _recv_msg_node->_cur_len += remain_msg;
        bytes_transferred -= remain_msg;
        copy_len += remain_msg;
        _recv_msg_node->_data[_recv_msg_node->_total_len] = '\0';
        spdlog::info("receive data: {}", _recv_msg_node->_data);
        //处理json
        Json::Reader reader;
        Json::Value root;
        reader.parse(std::string(_recv_msg_node->_data,_recv_msg_node->_total_len),root);

        send(_recv_msg_node->_data, _recv_msg_node->_total_len);
        _b_head_parse = false;
        _recv_head_node->clear();

        if (bytes_transferred <= 0) {
            std::memset(_data, 0, MAX_LENGTH);
            _socket.async_read_some(
                asio::buffer(_data, MAX_LENGTH),
                std::bind(&Session::handle_read, this, std::placeholders::_1,
                          std::placeholders::_2, _self_shared));
            return;
        }
        continue;
    }
}

void Session::handle_write(const system::error_code& error,
                           std::shared_ptr<Session> _self_shared) {
    if (error) {
        spdlog::error("write error, code: {}, message: {}", error.value(),
                      error.message());
        close();
        _server->clear_session(_uuid);
        return;
    }

    std::lock_guard<std::mutex> lock(_send_lock);
    _send_que.pop();
    if (!_send_que.empty()) {
        auto& msgNode = _send_que.front();
        spdlog::info("send data: {}", msgNode->_data + HEAD_TOTAL_LEN);
        asio::async_write(_socket,
                          asio::buffer(msgNode->_data, msgNode->_total_len),
                          std::bind(&Session::handle_write, this,
                                    std::placeholders::_1, _self_shared));
    }
}