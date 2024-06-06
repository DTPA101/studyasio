#include "server.h"
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <exception>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <csignal>

int main() {
    spdlog::set_level(spdlog::level::debug);
    try {
        boost::asio::io_context ioc;
        asio::signal_set signals(ioc,SIGINT,SIGTERM);
        signals.async_wait([&ioc](auto,auto){
            ioc.stop();
            spdlog::info("bye,bye");
        });
        Server s(ioc, 9999);
        ioc.run();
    } catch (const std::exception& e) {
        spdlog::error("exception: {}", e.what());
    }
    return 0;
}