cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)

#set(Boost_INCLUDE_DIR "/Users/paologastaldi/Downloads/boost_1_74_0")
#set(Boost_DIR "/Users/paologastaldi/Downloads/boost_1_74_0")
#set(Boost_LIBRARY_DIR "/Users/paologastaldi/Downloads/boost_1_74_0/lib")
#set(Boost_ROOT "/Users/paologastaldi/Downloads/boost_1_74_0")
#set(OPENSSL_INCLUDE_DIR "/usr/local/opt/openssl")

# --- client ---
project(client)

add_executable(client client/main.cpp client/FileWatcher.cpp client/FileWatcher.h client/FSElement.cpp client/FSElement.h comm/Message.cpp comm/Message.h client/Client.cpp client/Client.h)

find_package(Threads REQUIRED)
target_link_libraries(client Threads::Threads)

find_package(Boost REQUIRED)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(client ${Boost_LIBRARIES})
endif ()

find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    target_link_libraries(client ${OPENSSL_LIBRARIES})
endif ()

# --- comm ---

# add_executable(comm/API.cpp comm/API.h comm/Client_API.cpp comm/Client_API.h comm/Client_socket_API.h comm/Comm_error.cpp comm/Comm_error.h comm/Message.cpp comm/Message.h comm/Server_API.cpp comm/Server_API.h comm/Socket_API.cpp comm/Socket_API.h comm/Utils.cpp comm/Utils.h)

# --- server ---
project(server)

add_executable(server server/main.cpp server/Server.cpp server/Server.h server/Session.cpp server/Session.h)
find_package(Boost REQUIRED)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(server ${Boost_LIBRARIES})
endif()

target_link_libraries (server sqlite3)