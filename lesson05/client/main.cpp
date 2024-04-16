// #include <boost/asio/buffer.hpp>
// #include <boost/asio/error.hpp>
// #include <boost/asio/io_context.hpp>
// #include <boost/asio/ip/address.hpp>
// #include <boost/asio/ip/tcp.hpp>
// #include <boost/asio/read.hpp>
// #include <boost/asio/write.hpp>
// #include <cstddef>
// #include <cstring>
// #include <fmt/core.h>
// #include <iostream>
// #include <thread>
// //#include <json/json.h>
// using namespace std;
// using namespace boost::asio::ip;
// using namespace boost;

// constexpr int HEAD_LENGTH = 2;
// constexpr int MAX_LENGTH = 1024;

// int main() {
//     try {
//         // 创建上下文服务
//         asio::io_context ioc;
//         // 构造endpoint
//         tcp::endpoint remote_ep(address::from_string("127.0.0.1"), 9999);
//         tcp::socket sock(ioc);
//         boost::system::error_code error = asio::error::host_not_found;

//         sock.connect(remote_ep, error);
//         if (error) {
//             cout << "connect failed, code is " << error.value()
//                  << " error msg is " << error.message();
//             return 0;
//         }

//         std::thread send_thread([&sock] {
//             for (;;) {
//                 this_thread::sleep_for(std::chrono::milliseconds(2));
//                 std::cout<<"enter message\n";
//                 char request[MAX_LENGTH];
//                 std::cin.getline(request, MAX_LENGTH);
//                 short request_length = strlen(request);
//                 char send_data[MAX_LENGTH] = {0};
//                 short request_host_length = request_length;
//                 memcpy(send_data, &request_host_length, 2);
//                 memcpy(send_data + 2, request, request_length);
//                 asio::write(sock, asio::buffer(send_data, request_length + 2));
//             }
//         });

//         std::thread recv_thread([&sock] {
//             for (;;) {
//                 this_thread::sleep_for(std::chrono::milliseconds(2));
//                 cout << "begin to receive..." << endl;
//                 char reply_head[HEAD_LENGTH];
//                 size_t reply_length =
//                     asio::read(sock, asio::buffer(reply_head, HEAD_LENGTH));
//                 short msglen = 0;
//                 memcpy(&msglen, reply_head, HEAD_LENGTH);
//                 // 转为本地字节序
//                 char msg[MAX_LENGTH] = {0};
//                 size_t msg_length = asio::read(sock, asio::buffer(msg, msglen));

//                 std::cout << "Reply is: ";
//                 std::cout.write(msg, msglen) << endl;
//                 std::cout << "Reply len is " << msglen;
//                 std::cout << "\n";
//             }
//         });

//         send_thread.join();
//         recv_thread.join();
//     } catch (std::exception& e) {
//         std::cerr << "Exception: " << e.what() << endl;
//     }
//     return 0;
// }
#include <boost/asio/detail/socket_ops.hpp>
//#include <boost/asio/error.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <cstddef>
#include <cstring>
#include <exception>
#include <iostream>
#include <json/reader.h>
#include <json/value.h>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <string>

using namespace boost;
constexpr int MAX_LENGTH = 1024 * 2;
constexpr int HEAD_TOTAL_LEN = 4;
constexpr int HEAD_DATA_LEN = 2;
constexpr int HEAD_ID_LEN = 2;

int main() {
    spdlog::set_level(spdlog::level::debug);
    try {
        asio::io_context ioc;
        asio::ip::tcp::endpoint remote_ep(asio::ip::address::from_string("192.168.37.103"),
                                          9999);
        asio::ip::tcp::socket sock(ioc);
        boost::system::error_code error = asio::error::host_not_found;
        error = sock.connect(remote_ep, error);
        if (error) {
            spdlog::error("connect failed, code: {}, msg: {}",
                          error.value(),
                          error.message());
            return -1;
        }
        spdlog::info("connect to server: {}", remote_ep.address().to_string());

        std::string message;
        std::cout << "input message: ";
        std::cin >> message;
        Json::Value root;
        root["data"] = message;
        std::string request = root.toStyledString();
        std::size_t request_length = request.length();
        char send_data[MAX_LENGTH] = {0};
        int request_host_length = asio::detail::socket_ops::host_to_network_short(
            request_length);
        int msg_id = 1001;
        msg_id = asio::detail::socket_ops::host_to_network_short(msg_id);
        std::memcpy(send_data, &msg_id, HEAD_ID_LEN);
        std::memcpy(send_data + HEAD_ID_LEN, &request_host_length, HEAD_DATA_LEN);
        std::memcpy(send_data + HEAD_TOTAL_LEN, request.c_str(), request_length);
        asio::write(sock, asio::buffer(send_data, request_length + HEAD_TOTAL_LEN));

        char reply_head[HEAD_TOTAL_LEN];
        std::size_t reply_length = asio::read(sock,
                                              asio::buffer(reply_head, HEAD_TOTAL_LEN));

        msg_id = 0;
        std::memcpy(&msg_id, reply_head, HEAD_ID_LEN);
        short msg_len = 0;
        std::memcpy(&msg_len, reply_head + HEAD_ID_LEN, HEAD_DATA_LEN);
        msg_id = asio::detail::socket_ops::network_to_host_short(msg_id);
        msg_len = asio::detail::socket_ops::network_to_host_short(msg_len);
        char msg[MAX_LENGTH] = {0};
        std::size_t msg_length = asio::read(sock, asio::buffer(msg, msg_len));
        Json::Reader reader;
        reader.parse(std::string(msg, msg_length), root);
        spdlog::debug("receive msg id: {}, data: {}", msg_id, root["data"].asString());
        spdlog::info("client disconnect from the server");
    } catch (std::exception &e) {
        spdlog::error("Exception: {}", e.what());
    }
    return 0;
}