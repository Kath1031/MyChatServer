#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include <string>
#include <vector>
#include "group.hpp"

class GroupModel
{
public:
    // 创建群组
    bool CreateGroup(Group &group);
    // 加入群组
    void AddGroup(int user_id, int group_id, std::string role);
    // 查询用户所在群组信息
    std::vector<Group> QueryGroups(int user_id);
    // 根据指定group_id查询群用户id列表，除user_id自己
    std::vector<int> QueryGroupUsers(int user_id, int group_id);
};

#endif