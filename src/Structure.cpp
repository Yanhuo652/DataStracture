#include "../include/Structure.h"
#include "../include/UserManager.h"
#include <algorithm>

// 假设有一个全局的UserManager实例用于管理用户

User::User() : id(""), name(""), password(""), role(Role::Subscriber), tags(), subscribersCount(0) {}

// User带参构造函数实现
User::User(string id, string name, string password, Role role, vector<string> tags) : 
    id(id), name(name), password(password), role(role), tags(tags), subscribersCount(0) {
    }