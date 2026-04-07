#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

enum class Role {
    Admin, Creator, Subscriber
};

struct User {
    string id;
    string name;
    string password;
    Role role;
    vector<string> following;
    vector<string> followers;
    vector<string> tags;
    int subscribersCount;

    User();

    User(string id, string name, string password, Role role,vector<string> tags);


};

struct Post {
    int id;    
    string postId;
    string creatorId;
    string content;
    vector<string> tags;  // 帖子的标签
    long long timestamp;
    string status;  
    string userId;

    Post() :timestamp(0) , status("pending"){}
   
};



#endif