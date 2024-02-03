#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <unordered_map>
#include <functional>
#include <mutex>
#include "json.hpp"
#include "usermodel.hpp"
#include "offlinemsgmodel.hpp"
#include "friend_model.hpp"
#include "group_model.hpp"
#include "redis.hpp"
using json = nlohmann::json;

using MsgHandler = std::function<void(
    const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp)>;

// 聊天服务器业务类
class ChatService
{
public:
    // 获取单例对象的接口函数
    static ChatService *instance();
    // 登录
    void Login(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);
   
    //注销
    void LoginOut(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);
    // 注册
    void Reg(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);
    // 一对一聊天
    void OneChat(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);
    // 获取消息对应的处理器
    MsgHandler GetHandler(int msg_id);

    void Reset();

    void AddFriend(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);

    void ClientCloseException(const muduo::net::TcpConnectionPtr &conn);

    void CreateGroup(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);

    void AddGroup(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);

    void GroupChat(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);

    void HandleRedisSubsrcibeMessage(int user_id,std::string msg);
private:
    ChatService();
    // 存储消息id和其对应的业务处理方法
    std::unordered_map<int, MsgHandler> msg_handler_map_;

    // 存储在线用户的通信连接
    std::unordered_map<int, muduo::net::TcpConnectionPtr> user_conn_map_;

    std::mutex conn_mtx_;
    // 数据操作类对象
    UserModel user_model_;

    offlineMsgModel offline_msg_model_;

    FriendModel friend_model_;

    GroupModel group_model_;

    Redis redis_;
};
#endif