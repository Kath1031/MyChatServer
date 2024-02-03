#ifndef USERMODEL_H
#define USERMODEL_H
#include "user.hpp"
// User表的数据操作类
class UserModel
{
public:
    bool Insert(User &usr);
    User Query(int id);
    bool UpdateState(User user);
    void ResetState();
private:
};
#endif
