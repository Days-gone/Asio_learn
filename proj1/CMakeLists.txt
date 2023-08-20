cmake_minimum_required(VERSION 3.12)
project(qt_project)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories("./include")

find_package(RapidJSON CONFIG REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_executable(server src/svr.cpp src/data.cpp src/svr_example.cpp)
add_executable(client src/cli.cpp src/data.cpp src/cli_example.cpp)

target_link_libraries(server ${Boost_LIBRARIES})
target_link_libraries(client ${Boost_LIBRARIES})
target_link_libraries(server PRIVATE rapidjson)
target_link_libraries(client PRIVATE rapidjson)