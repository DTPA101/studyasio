cmake_minimum_required(VERSION 3.14)

project(asio)

find_package(Boost REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)

add_subdirectory(lesson01)
add_subdirectory(lesson02)
add_subdirectory(lesson03)
add_subdirectory(lesson04)
add_subdirectory(lesson05)
add_subdirectory(lesson06)
add_subdirectory(jsonstudy)
