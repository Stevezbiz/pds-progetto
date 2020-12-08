# REMOTE BACKUP

Client-server backup application using TCP socket connection. 

## Dependencies

    C++17: Robust support for most language features.
    Boost: Boost.Asio and some other parts of Boost.
    OpenSSL: Required for using TLS/Secure sockets and examples/tests
    Sqlite3: db required to the server to manage the login.


One of these components is required in order to build:

    CMake 3.16 or later
## Prerequisites
Building tests requires CMake, Boost, OpenSSL, sqlite3 installed.


### Ubuntu/Debian

Those dependencies can be simply installed.

```
sudo apt-get install cmake
sudo apt-get install libboost-all-dev
sudo apt install libssl-dev
sudo apt-get install libsqlite3-dev
```
### Mac OS

Using brew:
```
brew install cmake
brew install boost
brew install openssl
```

### Windows

* [CMake](https://cmake.org/)

  3.1+, consider using the latest version of CMake because it supports recent versions of libraries

* [Boost](https://www.boost.org)

  1.47+, the latest tested version is 1.74
  
## Build Instructions

Prerequisites: CMake, Boost, OpenSSL, sqlite3

```
$ cd build
$ ./build.sh
```
