cmake_minimum_required(VERSION 3.17)
project(client)

set(CMAKE_CXX_STANDARD 17)

add_executable(client client/main.cpp client/FileWatcher.cpp client/FileWatcher.h client/FSElement.cpp client/FSElement.h comm/Message.cpp comm/Message.h client/Client.cpp client/Client.h)

find_package(Threads REQUIRED)
target_link_libraries(client Threads::Threads)

find_package(Boost REQUIRED COMPONENTS filesystem)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(client ${Boost_LIBRARIES})
endif ()

find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    target_link_libraries(client ${OPENSSL_LIBRARIES})
endif ()

project(server)

set(CMAKE_CXX_STANDARD 17)

add_executable(server server/main.cpp comm/Message.cpp comm/Message.h server/Server.cpp server/Server.h server/Session.cpp server/Session.h)
find_package(Boost REQUIRED COMPONENTS timer thread)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(server ${Boost_LIBRARIES})
endif()

target_link_libraries (server sqlite3)