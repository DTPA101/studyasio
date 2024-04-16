#pragma once

class MsgNode {
public:
    MsgNode(short max_len);

    MsgNode(char* msg, int max_len);

    ~MsgNode();

    void clear();

    char* _data;
    short _cur_len;
    short _max_len;
    short _total_len;
};
