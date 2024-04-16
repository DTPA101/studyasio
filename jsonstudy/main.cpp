#include<json/json.h>
#include<json/reader.h>
#include<json/value.h>
#include<iostream>
int main(){
    Json::Value root;
    root["id"]=1001;
    root["data"]="hello world";
    std::string request=root.toStyledString();
    std::cout<<request<<std::endl;
    Json::Value root2;
    Json::Reader reader;
    reader.parse(request,root2);
    std::cout<<"id"<<root["id"].asInt()<<"msg"<<root2["data"]<<std::endl;
}