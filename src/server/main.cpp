
#include <iostream>
#include <memory>
#include <signal.h>
#include "chat_server.hpp"
#include "chat_service.hpp"
using namespace std;
void ResetHandler(int) {
    ChatService::instance()->Reset();
    exit(0);
}
int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cerr << "command invalid! example: ./ChatServer 127.0.0.1 6000" << std::endl;
        exit(-1);
    }
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);
    signal(SIGINT,ResetHandler);
    std::shared_ptr<muduo::net::EventLoop> loop=std::make_shared<muduo::net::EventLoop>();
    muduo::net::InetAddress addr(ip, port);
    ChatServer server(loop, addr, "ChatServer");
    server.Start();
    loop->loop();
    return 0;
}