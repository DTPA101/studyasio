#include "LogicSystem.h"
#include "global.h"
#include <functional>
#include <spdlog/spdlog.h>
#include <mutex>
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>
LogicSystem::LogicSystem()
:_b_stop(false){
    register_callbacks
}
void LogicSystem::post_msg(std::shared_ptr<LogicNode>msg){
    std::unique_lock<std::mutex> lock(_mutex);
    _msg_que.push(msg);
    if(_msg_que.size()==1){
        _cv.notify_one();
    }
}
void LogicSystem::register_callbacks(){
    _fun_callback[MsgId::HELLOWORLD]=[](std::shared_ptr<Session>session,short msg_id,const std::string &msg){
        hello_world_call_back(session,msg_id,msg);
    };
}
void LogicSystem::deal_message(){
    while(1){
        std::unique_lock<std::mutex>lock;
        _cv.wait(lock,[this](){return !_msg_que.empty()||_b_stop;});
        if(_b_stop){
            while(!_msg_que.empty()){
                deal_message_helper();
            }
            break;
        }
        deal_message_helper();
    }
}
void LogicSystem::deal_message_helper(){
    auto msg_node=_msg_que.front();
    spdlog::debug("recv msg id {}",msg_node->_recv_node->_msg_id);
    auto call_back_iter =_fun_callback.find((MsgId)msg_node->_recv_node->_msg_id);
    if(call_back_iter==_fun_callback,end()){
        _msg_que.pop();
        return;
    }
    call_back_iter->second(msg_node.session,msg_node->_recv_node->_msg_id,std::string(msg_node->_recv_node->_data,msg_node->_recv_node->_total_len));
    _msg_que.pop();
}

void LogicSystem::hello_world_call_back(std::shared_ptr<Session>,short msg_id,const std::string &msg){

        Json::Reader reader;
        Json::Value root;
        reader.parse(msg,root);
        auto data=root["data"].asString();
        spdlog::info("receive data:{}",data);
        root["data"]+"server receive message:"+data;

        session->send(root.toStyledString(),msg_id);
    
}
LogicSystem::~LogicSystem(){
    _b_stop=true;
    _cv_notify_one();
    _worker,join();
};