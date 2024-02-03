#include "friend_model.hpp"
#include "db.h"
#include <iostream>

void FriendModel::Insert(int user_id, int friend_id){
    // 组装sql语句
    char sql[1024] = {};
    sprintf(sql, "insert into friend value(%d,%d)",
            user_id,friend_id);

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
    return ;
}

std::vector<User> FriendModel::Query(int user_id){
    char sql[1024] = {};
    sprintf(sql,
            "select a.id,a.name,a.state from user a inner join friend b on b.friendid = a.id where b.userid=%d",
            user_id);

    MySQL mysql;
    std::vector<User> vec;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.SetId(atoi(row[0]));
                user.SetName(row[1]);
                user.SetState(row[2]);
                vec.push_back(user);
            }
            mysql_free_result(res);
        }
    }
    return vec;
}
