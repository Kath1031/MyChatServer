#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <functional>
#include <string>
using namespace std;
using namespace muduo;
using namespace muduo::net;

class ChatServer
{
public:
    ChatServer(EventLoop* loop
    ,const InetAddress& listenAddr,
    const string& nameArg)
    :_server(loop,listenAddr,nameArg)
    ,_loop(loop)
    {
        //给服务器注册用户链接的创建和断开回调
        _server.setConnectionCallback(std::bind(&ChatServer::onConnection,this,placeholders::_1));
        
        //给服务器注册用户读写事件回调
        _server.setMessageCallback(std::bind(&ChatServer::onMessage,this
        ,placeholders::_1,placeholders::_2,placeholders::_3));
        
        //1个IO线程，3个worker线程
        _server.setThreadNum(4);
    }

    void start() {
        _server.start();
    }
private:
    //处理用户的连接创建和断开
    void onConnection(const TcpConnectionPtr& conn) {
        if(conn->connected()) {
            std::cout << conn->peerAddress().toIpPort() << " -> " <<
                conn->localAddress().toIpPort() << " state:online" << endl;
        } else {
            std::cout << conn->peerAddress().toIpPort() << " -> " <<
                conn->localAddress().toIpPort() << " state:offline" << endl;
            conn->shutdown();
            //_loop->quit();
        }
    }

    //处理用户的读写事件
    void onMessage(const TcpConnectionPtr& conn,
    Buffer* buffer,
    Timestamp time)
    {
        std::string buf = buffer->retrieveAllAsString();
        std::cout << "recv data: " << buf << " time" << time.toString() << std::endl;
        conn->send(buf);
    }
    muduo::net::TcpServer _server;
    muduo::net::EventLoop *_loop;
};

int main()
{
    EventLoop loop;
    InetAddress addr("192.168.112.132",6000);
    ChatServer server(&loop,addr,"CharServer");
    server.start();
    loop.loop();
    return 0;
}