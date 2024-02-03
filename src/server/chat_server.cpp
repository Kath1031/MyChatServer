
#include "chat_server.hpp"
#include "chat_service.hpp"
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <memory>
#include <functional>
#include <string>
#include "json.hpp"

using json = nlohmann::json;

// 初始化聊天服务器对象
ChatServer::ChatServer(std::shared_ptr<muduo::net::EventLoop> loop,
                       const muduo::net::InetAddress &listenAddr,
                       const std::string &nameArg)
    : server_(loop.get(), listenAddr, nameArg), loop_(loop)
{
    // 注册链接回调
    server_.setConnectionCallback(std::bind(&ChatServer::OnConnection, this, std::placeholders::_1));

    // 注册消息回调
    server_.setMessageCallback(std::bind(&ChatServer::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // 设置线程数量
    server_.setThreadNum(4);
}

// 启动服务
void ChatServer::Start()
{
    server_.start();
}

// 上报链接相关信息的回调函数
void ChatServer::OnConnection(const muduo::net::TcpConnectionPtr &conn)
{
    // 客户端断开链接
    if (!conn->connected())
    {
        //ChatService::instance()->clientCloseException(conn);
        ChatService::instance() ->ClientCloseException(conn);
        conn->shutdown();
    }
}

// 上报读写事件相关信息的回调函数
void ChatServer::OnMessage(const muduo::net::TcpConnectionPtr &conn,
                           muduo::net::Buffer *buffer,
                           muduo::Timestamp time)
{
    std::string buf = buffer->retrieveAllAsString();

    // 数据的反序列化
    json js = json::parse(buf);

    auto MsgHandler=ChatService::instance()->GetHandler(js["msgid"].get<int>());
    
    MsgHandler(conn,js,time);


}