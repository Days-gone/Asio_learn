cmake_minimum_required(VERSION 3.22)
project(Chat_server)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories("./include")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_executable(server src/chat_svr.cpp src/Server.cpp src/Session.cpp)

target_link_libraries(server ${Boost_LIBRARIES})