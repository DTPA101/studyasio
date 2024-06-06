#pragma once
#include "Singleton.hpp"
#include "LogicNode.h"
#include <map>
#include <thread>
#include <memory>
#include <queue>
#include <functional>
#include <condition_variable>
#include "global.h"
class Session;
using FunCallback=std::function<void(std::shared_ptr<Session>,short,const std::string &)>;

class LogicSystem:public Singleton<LogicSystem>{
   public:
   LogicSystem();
   ~LogicSystem();
   void post_msg(std::shared_ptr<LogicNode> msg);
   private:
   void register_callbacks();
   void hello_world_call_back(std::shared_ptr<Session>,short msg_id,const std::string &msg);
   void deal_message();
   void deal_message_helper();
   std::queue<std::shared_ptr<LogicNode>> _msg_que;
   std::mutex _mutex;
   std::condition_variable _cv;
   std::thread _worker;
   bool _b_stop;
   std::unordered_map<MsgId,FunCallback> _fun_callback;
};