#pragma once
#include <memory>
#include "MsgNode.h"
#include "session.h"
class LogicNode{
    friend class LogicSystem;
    public:
     LogicNode(std::shared_ptr<Session>session,std::shared_ptr<RecvNode> recv_node);
    private:
    std::shared_ptr<Session>_session;
    std::shared_ptr<RecvNode>_recv_node;
};