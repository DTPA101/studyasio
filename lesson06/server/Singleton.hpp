#pragma once
#include<memory>
#include<mutex>

template<typename T>
class Singleton{
public:
     Singleton(const Singleton &)=delete;
     Singleton&operator=(const Singleton &)=delete;
     ~Singleton()=default;
     static std::shared_ptr<T>instance(){
        static std::once_flag _flag;
        std::call_once(_flag,[&](){_instance =std::make_shared<T>();});
        return _instance;
     }
protected:
     Singleton()=default;
private:
     static std::shared_ptr<T> _instance;
     
};
template<typename T>
std::shared_ptr<T> Singleton<T>::_instance=nullptr;