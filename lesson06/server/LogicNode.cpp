#include "LogicNode.h"
LogicNode::LogicNode(std::shared_ptr<Session>session,std::shared_ptr<RecvNode> recv_node)
:_session(session)
,_recv_node(recv_node){}