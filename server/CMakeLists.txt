cmake_minimum_required(VERSION 3.16)
project(server)

set(CMAKE_CXX_STANDARD 17)

add_executable(server main.cpp Message.cpp Message.h Server.cpp Server.h Session.cpp Session.h)
find_package(Boost REQUIRED COMPONENTS timer thread filesystem)


if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(server ${Boost_LIBRARIES})
endif()