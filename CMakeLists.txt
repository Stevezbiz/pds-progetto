cmake_minimum_required(VERSION 3.17)

#set(Boost_INCLUDE_DIR "/Users/paologastaldi/Downloads/boost_1_74_0")
#set(Boost_LIBRARY_DIR "/Users/paologastaldi/Downloads/boost_1_74_0/lib")
#set(Boost_ROOT_DIR "/Users/paologastaldi/Downloads/boost_1_74_0")
#set(OPENSSL_INCLUDE_DIR /usr/local/opt/openssl)

# --- comm ---
set(CMAKE_CXX_STANDARD 17)

SET(COMM_SOURCE_FILES
        comm/API.cpp
        comm/API.h
        comm/Client_API.cpp
        comm/Client_API.h
        comm/Client_socket_API.h
        comm/Comm_error.cpp
        comm/Comm_error.h
        comm/Message.cpp
        comm/Message.h
        comm/Server_API.cpp
        comm/Server_API.h
        comm/Socket_API.cpp
        comm/Socket_API.h
        comm/Utils.cpp
        comm/Utils.h
)

# --- stub client ---
project(stub_client)

set(CMAKE_CXX_STANDARD 17)

set(STUB_CLIENT_SOURCE_FILES
        client/stub_main.cpp
        client/Stub_client.cpp
        client/Stub_client.h
)
add_executable(stub_client ${STUB_CLIENT_SOURCE_FILES} ${COMM_SOURCE_FILES})

find_package(Threads REQUIRED)
target_link_libraries(stub_client Threads::Threads)

find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    target_link_libraries(stub_client ${OPENSSL_LIBRARIES})
else ()
    message(WARNING "Cannot find OpenSSL")
endif ()

# --- client ---
project(client)

set(CMAKE_CXX_STANDARD 17)

set(CLIENT_SOURCE_FILES
        client/main.cpp
        client/FileWatcher.cpp
        client/FileWatcher.h
        client/FSElement.cpp
        client/FSElement.h
        client/Client.cpp
        client/Client.h
        comm/Message.cpp
        comm/Message.h
        comm/Utils.cpp
        comm/Utils.h
)
add_executable(client ${CLIENT_SOURCE_FILES} ${COMM_SOURCE_FILES})

find_package(Threads REQUIRED)
target_link_libraries(client Threads::Threads)

find_package(Boost REQUIRED)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(client ${Boost_LIBRARIES})
else ()
    message(WARNING "Cannot find Boost")
endif ()

find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    target_link_libraries(client ${OPENSSL_LIBRARIES})
else ()
    message(WARNING "Cannot find OpenSSL")
endif ()

# --- server ---
project(server)

set(CMAKE_CXX_STANDARD 17)

set(SERVER_SOURCE_FILES 
        server/main.cpp
        server/Server.cpp
        server/Server.h
        server/Session.cpp
        server/Session.h
        comm/Message.cpp
        comm/Message.h
)
add_executable(server ${SERVER_SOURCE_FILES} ${COMM_SOURCE_FILES})
find_package(Boost REQUIRED)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(server ${Boost_LIBRARIES})
else ()
    message(WARNING "Cannot find Boost")
endif ()

find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    target_link_libraries(client ${OPENSSL_LIBRARIES})
else ()
    message(WARNING "Cannot find OpenSSL")
endif ()

target_link_libraries(server sqlite3)