cmake_minimum_required(VERSION 3.17)
project(boost_fs)

set(CMAKE_CXX_STANDARD 17)

add_executable(boost_fs main.cpp FileWatcher.cpp FileWatcher.h FSElement.cpp FSElement.h)

find_package(Boost REQUIRED COMPONENTS filesystem)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(boost_fs ${Boost_LIBRARIES})
endif()

find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    target_link_libraries(boost_fs ${OPENSSL_LIBRARIES})
endif()