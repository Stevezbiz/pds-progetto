//
// Created by Paolo Gastaldi on 29/11/2020.
//

#include <iostream>
#include <functional>
#include <utility>

class Server {
    std::function<int(int, int)> op = [](int a, int b) { return 0; };

public:
    explicit Server() = default;

     void set_op(std::function<int(int, int)> op) {
        this->op = std::move(op);
    }

    int do_it(int a, int b) {
        return this->op(a, b);
    }
};

class Session {
    int a, b;

public:
    static Server *server;

    Session(int a, int b) : a(a), b(b) {}

    static void set_server(Server *server) {
        Session::server = server;
    }

    [[nodiscard]] int compute() const {
        return Session::server->do_it(this->a, this->b);
    }

};

Server *Session::server{};

int main(int argc, char **argv) {
    auto server = new Server{};
    server->set_op([](int a, int b) { return a+b; });
    Session::server = server;

    Session s1{ 1, 3 };
    Session s2{ 5, 6 };

    std::cout << s1.compute() << std::endl;
    std::cout << s2.compute() << std::endl;

    return 0;
}