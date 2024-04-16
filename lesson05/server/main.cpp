#include "server.h"
#include <boost/asio/io_context.hpp>
#include <exception>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>

int main() {
    spdlog::set_level(spdlog::level::debug);
    try {
        boost::asio::io_context ioc;
        Server s(ioc, 9999);
        ioc.run();
    } catch (const std::exception& e) {
        spdlog::error("exception: {}", e.what());
    }
    return 0;
}