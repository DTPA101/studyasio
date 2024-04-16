project(lesson05)

add_executable(server5 
    server/main.cpp 
    server/server.cpp 
    server/session.cpp 
    server/MsgNode.cpp
)
add_executable(client5 client/main.cpp)

target_link_libraries(server5 PRIVATE 
    ${Boost_LIBRARIES} 
    spdlog::spdlog
    jsoncpp_lib
)

target_link_libraries(client5 PRIVATE 
    ${Boost_LIBRARIES} 
    spdlog::spdlog
    jsoncpp_lib
)



