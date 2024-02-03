#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <string>
#include <memory>

class ChatServer
{
public:
    ChatServer() = delete;
    ChatServer(std::shared_ptr<muduo::net::EventLoop> loop,
               const muduo::net::InetAddress &listenAddr, const std::string &nameArg);
    ~ChatServer()=default;

    // 启动服务
    void Start();

private:
    // 上报连接相关的回调函数
    void OnConnection(const muduo::net::TcpConnectionPtr &);
    void OnMessage(const muduo::net::TcpConnectionPtr &,
                   muduo::net::Buffer *, muduo::Timestamp);
    muduo::net::TcpServer server_;
    std::shared_ptr<muduo::net::EventLoop> loop_;
};

#endif