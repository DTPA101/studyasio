#include "MsgNode.h"
#include <cstring>
constexpr int HEAD_LEN = 2;
MsgNode::MsgNode(char* msg, int max_len)
    : _total_len(max_len + HEAD_LEN), _cur_len(0) {
    _data = new char[_total_len + 1];
    std::memcpy(_data, &max_len, HEAD_LEN);
    std::memcpy(_data + HEAD_LEN, msg, max_len);
    _data[_total_len] = '\0';
}

MsgNode::MsgNode(short max_len)
    : _total_len(max_len), _cur_len(0) {
    _data = new char[_total_len + 1];
}

MsgNode::~MsgNode() {
    delete[] _data;
}

void MsgNode::clear() {
    std::memset(_data, 0, _total_len);
    _cur_len = 0;
}