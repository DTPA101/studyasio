#include "MsgNode.h"
#include <cstring>
#include "global.h"
#include <boost/asio/detail/socket_ops.hpp>
#include <cstring>
MsgNode::MsgNode(short max_len)
    : _total_len(max_len), _cur_len(0) {
    _data = new char[_total_len + 1];
    _data[_total_len] = '\0';
}


MsgNode::~MsgNode() {
    delete[] _data;
}

void MsgNode::clear() {
    std::memset(_data, 0, _total_len);
    _cur_len = 0;
}
RecvNode::RecvNode(short max_len,short msg_id)
:MsgNode(max_len){};
SendNode::SendNode(const char*msg,short max_len,short msg_id)
:MsgNode(max_len+HEAD_TOTAL_LEN),_msg_id(msg_id){
    short msg_id_net=boost::asio::detail::socket_ops::host_to_network_short(msg_id);
    short msg_len_net=boost::asio::detail::socket_ops::host_to_network_short(max_len);
    std::memcpy(_data,&msg_id_net,HEAD_DATA_LEN);
    std::memcpy(_data+HEAD_ID_LEN,&msg_len_net,HEAD_DATA_LEN);
    std::memcpy(_data+HEAD_TOTAL_LEN,msg,max_len);

};