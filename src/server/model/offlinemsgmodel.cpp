#include "offlinemsgmodel.hpp"
#include "db.h"

#include <iostream>

void offlineMsgModel::insert(int user_id, std::string msg)
{
    // 组装sql语句
    char sql[1024] = {};
    sprintf(sql, "insert into offlinemessage value(%d,'%s')",
            user_id, msg.c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
    return;
}

void offlineMsgModel::remove(int user_id)
{
    // 组装sql语句
    char sql[1024] = {};
    sprintf(sql, "delete from offlinemessage where userid = %d",
            user_id);

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
    return;
}

std::vector<std::string> offlineMsgModel::query(int user_id)
{
    char sql[1024] = {};
    sprintf(sql, "select * from offlinemessage where userid = %d",
            user_id);

    MySQL mysql;
    std::vector<std::string> vec;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                vec.push_back(row[1]);
            }
            mysql_free_result(res);
        }
    }
    std::cout<<"query-->";
    for(auto &str:vec) {
        std::cout<<str<<" ";
    }
    std::cout<<std::endl;
    return vec;
}