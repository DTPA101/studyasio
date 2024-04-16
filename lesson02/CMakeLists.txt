project(lesson02)

add_executable(server2 server/main.cpp)
add_executable(client2 client/main.cpp)

target_link_libraries(server2 PRIVATE 
    ${Boost_LIBRARIES} 
    fmt::fmt-header-only
    spdlog::spdlog
)

target_link_libraries(client2 PRIVATE 
    ${Boost_LIBRARIES} 
    fmt::fmt-header-only
    spdlog::spdlog
)