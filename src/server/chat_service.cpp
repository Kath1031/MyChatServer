#include "chat_service.hpp"
#include "public.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <muduo/base/Logging.h>
ChatService *ChatService::instance()
{
    static ChatService service;
    return &service;
}
ChatService::ChatService()
{
    msg_handler_map_.insert({EnMsgType::LOGIN_MSG,
                             std::bind(&ChatService::Login, this, std::placeholders::_1,
                                       std::placeholders::_2, std::placeholders::_3)});
    msg_handler_map_.insert({EnMsgType::REG_MSG,
                             std::bind(&ChatService::Reg, this, std::placeholders::_1,
                                       std::placeholders::_2, std::placeholders::_3)});
    msg_handler_map_.insert({EnMsgType::ONE_CHAT_MSG,
                             std::bind(&ChatService::OneChat, this, std::placeholders::_1,
                                       std::placeholders::_2, std::placeholders::_3)});
    msg_handler_map_.insert({EnMsgType::ADD_FRIEND_MSG,
                             std::bind(&ChatService::AddFriend, this, std::placeholders::_1,
                                       std::placeholders::_2, std::placeholders::_3)});
    msg_handler_map_.insert({EnMsgType::LOGINOUT_MSG,
                             std::bind(&ChatService::LoginOut, this, std::placeholders::_1,
                                       std::placeholders::_2, std::placeholders::_3)});
    // 群组业务
    msg_handler_map_.insert({EnMsgType::CREATE_GROUP_MSG,
                             std::bind(&ChatService::CreateGroup, this, std::placeholders::_1,
                                       std::placeholders::_2, std::placeholders::_3)});
    msg_handler_map_.insert({EnMsgType::ADD_GROUP_MSG,
                             std::bind(&ChatService::AddGroup, this, std::placeholders::_1,
                                       std::placeholders::_2, std::placeholders::_3)});
    msg_handler_map_.insert({EnMsgType::GROUP_CHAT_MSG,
                             std::bind(&ChatService::GroupChat, this, std::placeholders::_1,
                                       std::placeholders::_2, std::placeholders::_3)});

    if (redis_.Connect())
    {
        redis_.InitNotifyHandler(std::bind(&ChatService::HandleRedisSubsrcibeMessage, this, std::placeholders::_1,
                                           std::placeholders::_2));
    }
    else
    {
        std::cout << "redis connect error" << std::endl;
    }
}
void ChatService::Login(
    const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
{
    int id = js["id"].get<int>();
    std::string pwd = js["password"];

    User user = user_model_.Query(id);
    if (user.GetId() != -1 && user.GetPassword() == pwd)
    {
        if (user.GetState() == "online")
        {
            json response;
            response["msgid"] = EnMsgType::LOGIN_MSG_AC;
            response["errno"] = 2;
            response["errmsg"] = "This account is using!";
            conn->send(response.dump());
        }
        else
        {
            // 登录成功
            {
                std::lock_guard<std::mutex> lock(conn_mtx_);
                user_conn_map_.insert({id, conn});
            }

            // id用户登录成功后,向redis订阅channel(id)
            redis_.Subscribe(id);

            user.SetState("online");
            user_model_.UpdateState(user);

            json response;
            response["msgid"] = EnMsgType::LOGIN_MSG_AC;
            response["errno"] = 0;
            response["id"] = user.GetId();
            response["name"] = user.GetName();

            std::vector<std::string> msg_vec = offline_msg_model_.query(id);
            if (!msg_vec.empty())
            {
                response["offlinemsg"] = msg_vec;
                offline_msg_model_.remove(id);
            }
            std::vector<User> friend_vec = friend_model_.Query(id);
            if (!friend_vec.empty())
            {
                response["friends"] = json::array();
                for (auto &user : friend_vec)
                {
                    json js;
                    js["id"] = user.GetId();
                    js["name"] = user.GetName();
                    js["state"] = user.GetState();
                    response["friends"].push_back(js.dump());
                }
            }
            conn->send(response.dump());
        }
    }
    else
    {
        json response;
        response["msgid"] = EnMsgType::LOGIN_MSG_AC;
        response["errno"] = 1;
        response["errmsg"] = "name or password is invaild!";
        conn->send(response.dump());
    }
}

void ChatService::Reg(
    const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
{
    LOG_INFO << "do reg service";
    std::string name = js["name"];
    std::string pwd = js["password"];
    User user;
    user.SetPassword(pwd);
    user.SetName(name);

    bool state = user_model_.Insert(user);

    if (state)
    {
        // 注册成功
        json response;
        response["msgid"] = EnMsgType::REG_MSG_AC;
        response["errno"] = 0;
        response["id"] = user.GetId();
        conn->send(response.dump());
    }
    else
    {
        // 注册失败
        json response;
        response["msgid"] = EnMsgType::REG_MSG_AC;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

MsgHandler ChatService::GetHandler(int msg_id)
{

    // 记录错误日志
    auto it = msg_handler_map_.find(msg_id);
    if (it == msg_handler_map_.end())
    {
        return [=](const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp) -> void
        {
            LOG_ERROR << " msgid: " << msg_id << " can not find handler!";
            return;
        };
    }
    else
    {
        return it->second;
    }
}
void ChatService::LoginOut(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
{
    int user_id = js["id"].get<int>();
    {
        std::lock_guard<std::mutex> lock(conn_mtx_);
        auto it = user_conn_map_.find(user_id);
        if (it != user_conn_map_.end())
        {
            user_conn_map_.erase(it);
        }
    }
    // 用户注销，在redis中取消订阅通道
    redis_.Unsubscribe(user_id);

    // 更新用户状态信息
    User user(user_id, "", "", "offline");
    user_model_.UpdateState(user);
}
void ChatService::ClientCloseException(const muduo::net::TcpConnectionPtr &conn)
{
    std::lock_guard<std::mutex> lock(conn_mtx_);
    User user;
    for (auto it : user_conn_map_)
    {
        if (it.second == conn)
        {
            user.SetId(it.first);
            user_conn_map_.erase(it.first);
            break;
        }
    }
    if (user.GetId() == -1)
    {
        return;
    }
    // 用户注销，在redis中取消订阅通道
    redis_.Unsubscribe(user.GetId());
    user.SetState("offline");
    user_model_.UpdateState(user);
}

void ChatService::OneChat(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
{
    int to_id = js["toid"].get<int>();
    {
        std::lock_guard<std::mutex> lock(conn_mtx_);
        auto it = user_conn_map_.find(to_id);
        if (it != user_conn_map_.end())
        {
            // to_id不在线
            it->second->send(js.dump());
            return;
        }
    }
    User user = user_model_.Query(to_id);
    if (user.GetState() == "online")
    {
        redis_.Publish(to_id, js.dump());
        return;
    }

    offline_msg_model_.insert(to_id, js.dump());
    std::cout << "ChatService::OneChat-->"
              << " from_id: " << js["id"].get<int>() << "  to_id: " << to_id << " json: " << js.dump() << std::endl;
}

void ChatService::Reset()
{
    // 把online状态的用户，设置成offline
    user_model_.ResetState();
}

void ChatService::AddFriend(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
{
    int user_id = js["id"].get<int>();

    int friend_id = js["friendid"].get<int>();

    friend_model_.Insert(user_id, friend_id);
}

void ChatService::CreateGroup(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
{
    int user_id = js["id"].get<int>();
    std::string name = js["groupname"];
    std::string desc = js["groupdesc"];

    Group group(-1, name, desc);
    if (group_model_.CreateGroup(group))
    {
        group_model_.AddGroup(user_id, group.GetId(), "creator");
    }
}

void ChatService::AddGroup(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
{
    int user_id = js["id"].get<int>();
    int group_id = js["groupid"].get<int>();
    group_model_.AddGroup(user_id, group_id, "normal");
}

void ChatService::GroupChat(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
{
    int user_id = js["id"].get<int>();
    int group_id = js["groupid"].get<int>();
    std::vector<int> user_id_vec = group_model_.QueryGroupUsers(user_id, group_id);
    std::lock_guard<std::mutex> lock(conn_mtx_);
    for (auto id : user_id_vec)
    {
        auto it = user_conn_map_.find(id);

        if (it != user_conn_map_.end())
        {
            it->second->send(js.dump());
        }
        else
        {
            User user = user_model_.Query(id);
            if (user.GetState() == "online")
            {
                redis_.Publish(id, js.dump());
            }
            else
            {
                offline_msg_model_.insert(id, js.dump());
            }
        }
    }
}
void ChatService::HandleRedisSubsrcibeMessage(int user_id,std::string msg){
    std::lock_guard<std::mutex> lock(conn_mtx_);
    auto it = user_conn_map_.find(user_id);
    if(it!=user_conn_map_.end()) {
        it->second->send(msg);
        return ;
    }
    offline_msg_model_.insert(user_id,msg);
}