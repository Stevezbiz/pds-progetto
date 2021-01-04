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
```
#### build server 
```
$ ./build_s.sh
```
#### build client 
```
$ ./build_c.sh
```
## Usage 

- Server

  ```
  ./server
  ```

  - The backup directory folders for each user depend on the configuration parameters (these can be set at first boot default directory is ./server_files)
    

- Client 

  ```
  ./client
  ```
  - As for the server, the path for executing remote backup depends on the configuration parameters
## Description

### Client
  - It is not necessary to pass arguments, it is possible to insert the configuration parameters (_server ip_, _port_, _path_ ) at the first start.
  - A socket has been created and there is an attempt to connect with the server at the given port.
  - After the login phase, if it is successful, a user can decide if **_recover all_** files and folders from the Remote Backup Server or decide to keeps the remote backup server updated of any changes (**_normal mode_**), the normal mode is anticipated from a **_probe phase_** where the client can verify if the server already has every folder and file of users updated.
  - After initial settings is actived a system watcher that notices all the changes (Erased / Created or Modified) in the path provided by the user. Each element (file or folder) is represented by SHA256 hash + path name.
  - The parallelization of Client is based on 4 threads, the application operates on the socket managing the communication in a concurrent way, potentially it can send 4 files using 4 different threads. (the limit on the client is set for performance reasons, also the tests emphasized that no further was needed).


### Server
  - The endpoint is connected to the port.
  - The system can accept multiple clients, for each is created a thread where is created a class session (that expires in case of inactivity).
  - Server manages the login, a user provides their own username and password and the server checks the database and queries it if users are saved in the system.
  - The server handles receiving requests using 32 threads, if all threads are busy, discards the request and the socket is closed, the client detects the server busy and retries the connection, until the connection timeout expires, at this point the client closes the socket and terminate execution (client side).

    -  The accepted request generates the opening of the socket with its timer. (the life of a socket is fixed at 5 minutes: it is a reasonable amount of time, a good compromise between the protection of the Dos attack and the time to send large packets).
### Comm
- Server and client APIs shared all methods of the Message class that is the basis of the communication, it provides all communication functionality as send, probe, get, push, restore, build_content, build_header, get_header_buffer ...

- Both the server and the user have a dedicated APIs that they can use to contact each other.
    - e.g. the user just calls (login, push, restore..) and all operations are done correctly, the complexity is hidden in the APIs.
    #### Logger
    Logger is the instrumect that provide a complete document that contains all of the logs that are needed to record all events, happenings, and occurrences relative to the activity.
    There are 3 types of logs:
    - INFO
    - WARNING
    - ERROR

    They are organized in priority level from very-low to very-high so it is easier to check the importance of problems or general information of what is happening.


---

## Transfer protocol

##### Message
The Message is composed using a TLV format (type, length, value):
the message types are: 
  - MSG_ERROR
  - MSG_UNDEFINED
  - MSG_OKAY
  - MSG_LOGIN
  - MSG_PROBE
  - MSG_PROBE_CONTENT
  - MSG_GET
  - MSG_GET_CONTENT
  - MSG_PUSH
  - MSG_RESTORE
  - MSG_RESTORE_CONTENT
  - MSG_END = 100

  All the information are serialized and are emplaced in the socket: type and length are pushed in the header and all the file value are positioned in the content of the socket.
 Each message exchanged from a client maintains cookies that identify the session.
##### Socket
Here are used the boost asio sockets, they need ip of the server and port to be created.
Content usage is anticipated by function "call_" that allows identifying socket invalidity or failure operations, in that case, the function retries every 500 ms the connection until a preset number of repetitions.
At the reception, the socket is decomposed and the Message is reconstructed.
The sending phase is composed of the writing of the Message in the socket using asio writing.
The socket has a Timeout that when it expires the session is closed.
##### Session
Each client is identified by a server-generated __cookie__.
The session doesn't allow any operation if the login is not completed successfully.

- From the server-side is managed the creation and the expiration of the session, if a client doesn't send anything for a given time, the session is pulled down and the socket closed, if the client tries to send something it is recreated a session with a given client's cookie.
Sessions are managed by the Session manager who takes care of the verify of the cookies, retrieves a session in case a session expires after the timeout and it creates new sessions.

- From the client-side check if there are a connection error status -> in this case probably the session was session dropped from the server, it retries and opens the connection trying a recontact the server from a new session created from the other side (server).


---
### Testing

Here are reported some general tests used to evaluate the robustness of the application protocol.
Please refer to [tests](https://docs.google.com/document/d/1il2Jn_PaOJgVW9Q2TIM7fdbESWLrJWPBcLl2pLu8cuM/edit?usp=sharing).

## Contributors
 The application has been developed by:
- Borsarelli Carlo 
- Gastaldi Paolo 
- Gennero Stefano 
