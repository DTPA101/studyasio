project(lesson06)

add_executable(server6 
    server/main.cpp 
    server/server.cpp 
    server/session.cpp 
    server/MsgNode.cpp
    server/LogicNode.cpp
    server/Singleton.hpp
    server/AsioIOContextPool.cpp
)
add_executable(client6 client/main.cpp)

target_link_libraries(server6 PRIVATE 
    ${Boost_LIBRARIES} 
    spdlog::spdlog
    jsoncpp_lib
)

target_link_libraries(client6 PRIVATE 
    ${Boost_LIBRARIES} 
    spdlog::spdlog
    jsoncpp_lib
)



