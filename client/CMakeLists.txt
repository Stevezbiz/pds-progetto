cmake_minimum_required(VERSION 3.16)
project(client)

set(CMAKE_CXX_STANDARD 17)

add_executable(client main.cpp FileWatcher.cpp FileWatcher.h FSElement.cpp FSElement.h Message.cpp Message.h Client.cpp Client.h)

find_package(Threads REQUIRED)
target_link_libraries(client Threads::Threads)

find_package(Boost REQUIRED COMPONENTS filesystem)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(client ${Boost_LIBRARIES})
endif()

find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    target_link_libraries(client ${OPENSSL_LIBRARIES})
endif()