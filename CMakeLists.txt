cmake_minimum_required(VERSION 3.17)

if (APPLE)
    message(STATUS "Using Apple configuration paths")

    # Boost configuration
    set(Boost_INCLUDE_DIR "/Users/paologastaldi/Downloads/boost_1_74_0/") # the only path needed for Boost

    set(Boost_INCLUDE_DIRS "/Users/paologastaldi/Downloads/boost_1_74_0/")
    set(Boost_LIBRARY_DIRS "/Users/paologastaldi/Downloads/boost_1_74_0/boost/")
    set(Boost_LIBRARIES "/Users/paologastaldi/Downloads/boost_1_74_0/libs/")

#    set(Boost_ROOT_DIR /Users/paologastaldi/Downloads/boost_1_74_0)
#    set(Boost_DIR ${Boost_ROOT_DIR})
#    set(Boost_LIBRARY_DIR ${Boost_ROOT_DIR}/libs)
#    set(Boost_LIBRARY_DIRS ${Boost_LIBRARY_DIR})
#    set(BOOST_LIBRARYDIR ${Boost_LIBRARY_DIR})
#    set(Boost_LIBRARIES ${Boost_LIBRARY_DIR})
#
#    set(Boost_INCLUDE_DIR ${Boost_ROOT_DIR})
#    set(Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIR} ${Boost_LIBRARY_DIRS})
#    set(BOOST_INCLUDEDIR ${Boost_INCLUDE_DIR})

#     SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${Boost_ROOT_DIR})
#     SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${Boost_LIBRARY_DIR})
#     set(BOOST_SEARCH_PATHS ${Boost_ROOT_DIR} ${Boost_INCLUDE_DIR} ${Boost_LIBRARY_DIR} ${Boost_LIBRARIES})

    # openSSL configuration
    set(openssl_INCLUDE_DIR /usr/local/Cellar/openssl@1.1/1.1.1h/include)
    set(OPENSSL_INCLUDE_DIR ${openssl_INCLUDE_DIR})

#    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} /usr/local/Cellar/openssl@1.1/1.1.1h/include/)
#    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/local/Cellar/openssl@1.1/1.1.1h/lib/)
    set(openssl_ROOT_DIR /usr/local/Cellar/openssl@1.1/1.1.1h/)
    set(OPENSSL_ROOT_DIR ${openssl_ROOT_DIR})
    set(openssl_INCLUDE_DIRS ${openssl_INCLUDE_DIR})
    set(OPENSSL_INCLUDE_DIRS ${openssl_INCLUDE_DIRS})
    set(openssl_LIBRARY_DIR /usr/local/Cellar/openssl@1.1/1.1.1h/lib/)
    set(OPENSSL_LIBRARY_DIR ${openssl_LIBRARY_DIR})
    set(openssl_LIBRARIES ${openssl_LIBRARY_DIR})
    set(OPENSSL_LIBRARIES ${openssl_LIBRARIES})
    set(openssl_LIBRARY_DIR ${openssl_LIBRARY_DIR})
    set(OPENSSL_LIBRARY_DIR ${openssl_LIBRARY_DIR})
    set(openssl_CRYPTO_LIBRARY ${openssl_ROOT_DIR}/lib/libcrypto.dylib)
    set(OPENSSL_CRYPTO_LIBRARY ${openssl_CRYPTO_LIBRARY})

    message(STATUS "Using Apple configuration paths - done")
endif()

set(CMAKE_CXX_STANDARD 17)

# set all the different includes
set(CLIENT_COMM_SOURCE_FILES
        comm/API.cpp
        comm/API.h
        comm/Client_API.cpp
        comm/Client_API.h
        comm/Client_socket_API.cpp
        comm/Client_socket_API.h
        comm/Comm_error.cpp
        comm/Comm_error.h
        comm/ElementStatus.h
        comm/Logger.cpp
        comm/Logger.h
        comm/Message.cpp
        comm/Message.h
        comm/Socket_API.cpp
        comm/Socket_API.h
        comm/Utils.cpp
        comm/Utils.h
        )

set(SERVER_COMM_SOURCE_FILES
        comm/API.cpp
        comm/API.h
        comm/Comm_error.cpp
        comm/Comm_error.h
        comm/ElementStatus.h
        comm/Logger.cpp
        comm/Logger.h
        comm/Message.cpp
        comm/Message.h
        comm/Server_API.cpp
        comm/Server_API.h
        comm/Socket_API.cpp
        comm/Socket_API.h
        comm/Utils.cpp
        comm/Utils.h
        )

set(STUB_CLIENT_SOURCE_FILES
        client/stub_main.cpp
        client/Stub_client.cpp
        client/Stub_client.h
        client/FileWatcher.cpp
        client/FileWatcher.h
        client/FSElement.cpp
        client/FSElement.h
        )

set(CLIENT_SOURCE_FILES
        client/main.cpp
        client/FileWatcher.cpp
        client/FileWatcher.h
        client/FSElement.cpp
        client/FSElement.h
        client/Client.cpp
        client/Client.h
        client/ConfigSetting.cpp
        client/ConfigSetting.h
        )

set(STUB_SERVER_SOURCE_FILES
        server/stub_main.cpp
        server/Session.cpp
        server/Session.h
        )

set(SERVER_SOURCE_FILES
        server/main.cpp
        server/Database_API.cpp
        server/Database_API.h
        server/Server.cpp
        server/Server.h
        server/Session.cpp
        server/Session.h
        server/Session_manager.cpp
        server/Session_manager.h
        )

### stub client ###
project(stub_client)
message(STATUS "preparing stub_client...")
# add includes
add_executable(stub_client ${STUB_CLIENT_SOURCE_FILES} ${CLIENT_COMM_SOURCE_FILES})
# add Boost
find_package(Boost REQUIRED filesystem serialization thread)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(stub_client ${Boost_LIBRARIES})
else ()
    message(WARNING "Cannot find Boost")
endif ()
# add Openssl
find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    include_directories(${openssl_INCLUDE_DIR})
    target_link_libraries(stub_client ${OPENSSL_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARY})
else ()
    message(WARNING "Cannot find OpenSSL")
endif ()
message(STATUS "preparing stub_client... - done")

### client ###
project(client)
message(STATUS "preparing client...")
# add includes
add_executable(client ${CLIENT_SOURCE_FILES} ${CLIENT_COMM_SOURCE_FILES})
# add Boost
find_package(Boost COMPONENTS filesystem serialization thread REQUIRED)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(client ${Boost_LIBRARIES})
else ()
    message(WARNING "Cannot find Boost")
endif ()
# add Openssl
find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    include_directories(${openssl_INCLUDE_DIR})
    target_link_libraries(client ${OPENSSL_LIBRARIES})
else ()
    message(WARNING "Cannot find OpenSSL")
endif ()
message(STATUS "preparing client... - done")

### stub server ###
project(stub_server)
message(STATUS "preparing stub_server...")
# add includes
add_executable(stub_server ${STUB_SERVER_SOURCE_FILES} ${SERVER_COMM_SOURCE_FILES})
find_package(Boost REQUIRED filesystem serialization thread)
# add Boost
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(stub_server ${Boost_LIBRARIES})
else ()
    message(WARNING "Cannot find Boost")
endif ()
# add Openssl
find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    include_directories(${openssl_INCLUDE_DIR})
    target_link_libraries(stub_server ${OPENSSL_LIBRARIES})
else ()
    message(WARNING "Cannot find OpenSSL")
endif ()
message(STATUS "preparing stub_server... done")

### server ###
project(server)
message(STATUS "preparing server...")
# add includes
add_executable(server ${SERVER_SOURCE_FILES} ${SERVER_COMM_SOURCE_FILES})
find_package(Boost REQUIRED filesystem serialization thread)
# add Boost
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(server ${Boost_LIBRARIES})
else ()
    message(WARNING "Cannot find Boost")
endif ()
# add Openssl
find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    include_directories(${openssl_INCLUDE_DIR})
    target_link_libraries(server ${OPENSSL_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARY})
else ()
    message(WARNING "Cannot find OpenSSL")
endif ()
# add Sqlite3
target_link_libraries(server sqlite3)
message(STATUS "preparing server... - done")

# --- test_serialization ---
project(test_serialization)
message(STATUS "preparing test_serialization...")

set(test_serialization_SOURCE_FILES
        test/serialization/test_serialization.cpp
        )

add_executable(test_serialization ${test_serialization_SOURCE_FILES})
find_package(Boost REQUIRED filesystem serialization thread)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(test_serialization ${Boost_LIBRARIES})
else ()
    message(WARNING "Cannot find Boost")
endif ()
message(STATUS "preparing test_serialization... - done")

# --- test_static ---
project(test_static)
message(STATUS "preparing test_static...")

set(test_static_SOURCE_FILES
        test/static/test_static.cpp
        )

add_executable(test_static ${test_static_SOURCE_FILES})
find_package(Boost REQUIRED)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(test_static ${Boost_LIBRARIES})
else ()
    message(WARNING "Cannot find Boost")
endif ()
message(STATUS "preparing test_static... - done")

# --- test_socket ---
project(test_socket)
message(STATUS "preparing test_socket...")

set(test_static_SOURCE_FILES
        test/socket/test_socket.cpp
        )

add_executable(test_socket ${test_static_SOURCE_FILES})
find_package(Boost REQUIRED thread)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(test_socket ${Boost_LIBRARIES})
else ()
    message(WARNING "Cannot find Boost")
endif ()
message(STATUS "preparing test_socket... - done")
