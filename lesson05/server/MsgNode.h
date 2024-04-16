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
class RecvNode:public MsgNode{
    public:
    RecvNode(short max_len,short msg_id);
    short _msg_id;
};
class SendNode:public MsgNode{
public:
   SendNode(const char *msg ,short max_len,short msg_id);
private:
short _msg_id; 
};
