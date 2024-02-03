#ifndef USER_H
#define USER_H
#include <string>
//User表的ORM类
class User
{
private:
    int id_;
    std::string name_;
    std::string password_;
    std::string state_;

public:
    User(int id = -1, std::string name = "", std::string password = "", std::string state = "offline")
        : id_(id), name_(name), password_(password), state_(state)
    {}
    void SetId(int id) { this->id_ = id; }
    void SetName(std::string name) { this->name_ = name; }
    void SetPassword(std::string password) { this->password_ = password; }
    void SetState(std::string state) { this->state_ = state; }

    int GetId() const { return this->id_; }
    std::string GetName() const { return this->name_; }
    std::string GetPassword() const {return this->password_;}
    std::string GetState() const {return this->state_;}
};

#endif