#include "usermodel.hpp"
#include "db.h"
#include <iostream>
#include <memory>
#include <string>
bool UserModel::Insert(User &user)
{
    // 组装sql语句
    char sql[1024] = {};
    sprintf(sql, "insert into user(name, password, state) value('%s','%s','%s')",
            user.GetName().c_str(), user.GetPassword().c_str(), user.GetState().c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            user.SetId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}

User UserModel::Query(int id)
{
    // 组装sql语句
    char sql[1024] = {};
    sprintf(sql, "select * from user where id = %d", id);

    MySQL mysql;
    User user = User();
    if (mysql.connect())
    {
        MYSQL_RES* res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                user.SetId(atoi(row[0]));
                user.SetName(row[1]);
                user.SetPassword(row[2]);
                user.SetState(row[3]);
                mysql_free_result(res);
                return user;
            }
        }
    }
    return user;
}

bool UserModel::UpdateState(User user)
{
    // 组装sql语句
    char sql[1024] = {};
    sprintf(sql, "update user set state = '%s' where id = %d",
            user.GetState().c_str(), user.GetId());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            return true;
        }
    }
    return false;
}
void UserModel::ResetState(){
        // 组装sql语句
    char sql[1024] = "update user set state = 'offline' where state = 'online'";

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}