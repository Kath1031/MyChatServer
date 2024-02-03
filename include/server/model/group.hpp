#ifndef GROUP_H
#define GROUP_H
#include <string>
#include <vector>
#include <group_user.hpp>
class Group
{
public:
    Group(int id = -1, std::string name = "", std::string desc = "")
        : id_(id), name_(name), desc_(desc) {}

    void SetId(int id) { id_ = id; }
    void SetName(std::string name) { name_ = name; }
    void SetDesc(std::string desc) { desc_ = desc; }

    int GetId() { return id_; }
    std::string GetName() { return name_; }
    std::string GetDesc() { return desc_; }
    std::vector<GroupUser> &GetUsers() { return users_; }

private:
    int id_;
    std::string name_;
    std::string desc_;
    std::vector<GroupUser> users_;
};

#endif