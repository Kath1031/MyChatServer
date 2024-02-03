#include "group_model.hpp"
#include "db.h"

bool GroupModel::CreateGroup(Group &group)
{
    // 组装sql语句
    char sql[1024] = {};
    sprintf(sql, "insert into allgroup(groupname, groupdesc) value('%s','%s')",
            group.GetName().c_str(), group.GetDesc().c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            group.SetId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}

void GroupModel::AddGroup(int user_id, int group_id, std::string role)
{
    // 组装sql语句
    char sql[1024] = {};
    sprintf(sql, "insert into groupuser value(%d,%d,'%s')",
            group_id, user_id, role.c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
    return;
}

std::vector<Group> GroupModel::QueryGroups(int user_id)
{
    // 组装sql语句
    char sql[1024] = {};
    sprintf(sql, "select a.id,a.groupname,a.groupdesc from allgroup a inner join \
    groupuser b on a.id = b.groupid where b.userid = %d",
            user_id);
    std::vector<Group> group_vec;
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                Group group;
                group.SetId(atoi(row[0]));
                group.SetName(row[1]);
                group.SetDesc(row[2]);
                group_vec.push_back(group);
            }
            mysql_free_result(res);
        }
    }

    for (auto &group : group_vec)
    {
        sprintf(sql, "select a.id,a.name,a.state,b.grouprole from user a \
        inner join groupuser b on b userid = a.id where b.groupid = %d",
                group.GetId());
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                GroupUser user;
                user.SetId(atoi(row[0]));
                user.SetName(row[1]);
                user.SetState(row[2]);
                user.SetRole(row[3]);
                group.GetUsers().push_back(user);
            }
            mysql_free_result(res);
        }
    }

    return group_vec;
}

std::vector<int> GroupModel::QueryGroupUsers(int user_id, int group_id)
{
    // 组装sql语句
    char sql[1024] = {};
    sprintf(sql, "select userid from groupuser where groupid = %d and userid != %d",
            group_id, user_id);
    std::vector<int> id_vec;
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if(res !=nullptr) {
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res))!=nullptr){
                id_vec.push_back(atoi(row[0]));
            }
            mysql_free_result(res);

        }
    }

    return id_vec;
}