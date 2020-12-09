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

* [Boost](https://www.boost.org/users/download/)

  - Download boost( or zip) and unzip somewhere.
* [OpenSSL](https://slproweb.com/products/Win32OpenSSL.html)
  - Download Win32 OpenSSL v1.0.2h and Win64 OpenSSL v1.0.2h
  - Run Win32OpenSSL-1_0_2h.exe and Win64OpenSSL-1_0_2h.exe and install to openssl directory(ex. C\lib\openssl)
  
## Build Instructions

```
$ cd build
$ ./build.sh
```

## Usage 

- Server

  ```
  ./server <Port>
  ```

  - Backup directory
    backup folders for each user
    /server/???
    

- Client 

  ```
  ./client
  ```

## Description

### Client
It is not necessary to pass arguments, it is possible to insert the configuration parameter at the first start on the command line.



 
