project(lesson03)

add_executable(server3 server/main.cpp server/server.cpp server/session.cpp)

target_link_libraries(server3 PRIVATE 
    ${Boost_LIBRARIES} 
    spdlog::spdlog
)

