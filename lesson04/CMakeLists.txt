project(lesson04)

add_executable(server4 
    server/main.cpp 
    server/server.cpp 
    server/session.cpp 
    server/MsgNode.cpp
)
add_executable(client4 client/main.cpp)


target_link_libraries(server4 PRIVATE 
    ${Boost_LIBRARIES} 
    spdlog::spdlog
)

target_link_libraries(client4 PRIVATE 
    ${Boost_LIBRARIES} 
    spdlog::spdlog
)

