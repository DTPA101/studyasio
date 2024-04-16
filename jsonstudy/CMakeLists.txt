project(jsonstudy)

find_package(jsoncpp REQUIRED)

add_executable(jsoncpp_example main.cpp)
target_link_libraries(jsoncpp_example PRIVATE JsonCpp::JsonCpp)