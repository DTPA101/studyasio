#pragma once
#include "Singleton.hpp"
#include <boost/asio/io_context.hpp>
#include <cstddef>
#include <memory>
#include <thread>
#include <vector>
namespace asio=boost::asio;
class AsioIOContextPool :public Singleton<AsioIOContextPool>{
    using IOContext=boost::asio::io_context;
    using Work=boost::asio::io_context::work;
    using  WorkPtr=std::unique_ptr<Work>;
    public:
    explicit AsioIOContextPool(std::size_t size=std::thread::hardware_concurrency()-1);
    ~AsioIOContextPool()=default;
    asio::io_context& get_io_context();
    void stop();
    private:
    std::vector<IOContext>_io_contexts;
    std::vector<WorkPtr>_works;
    std::vector<std::thread>_threads;
    std::size_t _next_io_context;
};