cmake_minimum_required(VERSION 3.12)
project(cpp_network)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories("./include")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# add_executable(server src/async_server.cpp src/temp.cpp)
add_executable(client src/sync_client.cpp)

# target_link_libraries(server ${Boost_LIBRARIES})
target_link_libraries(client ${Boost_LIBRARIES})