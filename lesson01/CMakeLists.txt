project(lesson01)

add_executable(${PROJECT_NAME} endpoint.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE 
    ${Boost_LIBRARIES} 
    fmt::fmt-header-only 
    spdlog::spdlog
)