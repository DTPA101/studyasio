#include "AsioIOContextPool.h"
#include <memory>
#include <cstddef>
AsioIOContextPool::AsioIOContextPool(std::size_t size)
:_io_contexts(size),_works(size),_next_io_context(0){
    for(std::size_t i=0;i<size;++i){
        _works[i]=std::make_unique<Work>(_io_contexts[i]);
    }
    for(std::size_t i=0;i<size;++i){
        _threads.emplace_back([this,i]->void{_io_contexts[i].run();});
    }
}
asio::io_context &AsioIOContextPool::get_io_context(){
   auto& context =_io_contexts[_next_io_context++];
   if(_next_io_context==_io_contexts.size()){
    _next_io_context=0;
    return context;
   }
}
void AsioIOContextPool::stop(){
   for(auto& work:_works){
    work->get_io_context().stop();
    work.reset();
   }
   for(auto &t:_threads){
    t.join();
   }
}