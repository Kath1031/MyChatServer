#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H

#include "user.hpp"
#include <vector>
class FriendModel
{
public:
    void Insert(int user_id, int friend_id);

    // 返回用户好友列表
    std::vector<User> Query(int user_id);
};

#endif