#ifndef PUBLIC_H
#define PUBLIC_H

enum EnMsgType
{
    LOGIN_MSG = 1, // 登录消息
    LOGIN_MSG_AC,
    LOGINOUT_MSG,
    REG_MSG,
    REG_MSG_AC,
    ONE_CHAT_MSG,
    ADD_FRIEND_MSG,

    CREATE_GROUP_MSG, // 创建群组
    ADD_GROUP_MSG,    // 加入群组
    GROUP_CHAT_MSG,   // 群聊天

};
#endif