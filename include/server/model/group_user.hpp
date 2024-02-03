#ifndef GROUPUSER_H
#define GROUPUSER_H

#include "user.hpp"
class GroupUser : public User
{
public:
    void SetRole(std::string role) { role_ = role; }
    std::string GetRole(){return role_;}

private:
    std::string role_;
};

#endif