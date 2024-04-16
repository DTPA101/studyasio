#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <cstddef>
#include <cstring>
#include <fmt/core.h>
#include <iostream>
#include <thread>
//#include <json/json.h>
using namespace std;
using namespace boost::asio::ip;
using namespace boost;

constexpr int HEAD_LENGTH = 2;
constexpr int MAX_LENGTH = 1024;

int main() {
    try {
        // 创建上下文服务
        asio::io_context ioc;
        // 构造endpoint
        tcp::endpoint remote_ep(address::from_string("127.0.0.1"), 9999);
        tcp::socket sock(ioc);
        boost::system::error_code error = asio::error::host_not_found;

        sock.connect(remote_ep, error);
        if (error) {
            cout << "connect failed, code is " << error.value()
                 << " error msg is " << error.message();
            return 0;
        }

        std::thread send_thread([&sock] {
            for (;;) {
                this_thread::sleep_for(std::chrono::milliseconds(2));
                std::cout<<"enter message\n";
                char request[MAX_LENGTH];
                std::cin.getline(request, MAX_LENGTH);
                short request_length = strlen(request);
                char send_data[MAX_LENGTH] = {0};
                short request_host_length = request_length;
                memcpy(send_data, &request_host_length, 2);
                memcpy(send_data + 2, request, request_length);
                asio::write(sock, asio::buffer(send_data, request_length + 2));
            }
        });

        std::thread recv_thread([&sock] {
            for (;;) {
                this_thread::sleep_for(std::chrono::milliseconds(2));
                cout << "begin to receive..." << endl;
                char reply_head[HEAD_LENGTH];
                size_t reply_length =
                    asio::read(sock, asio::buffer(reply_head, HEAD_LENGTH));
                short msglen = 0;
                memcpy(&msglen, reply_head, HEAD_LENGTH);
                // 转为本地字节序
                char msg[MAX_LENGTH] = {0};
                size_t msg_length = asio::read(sock, asio::buffer(msg, msglen));

                std::cout << "Reply is: ";
                std::cout.write(msg, msglen) << endl;
                std::cout << "Reply len is " << msglen;
                std::cout << "\n";
            }
        });

        send_thread.join();
        recv_thread.join();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << endl;
    }
    return 0;
}