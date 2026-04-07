#ifndef USERMANAGER_H
#define USERMANAGER_H

#include "Structure.h"
#include <vector>
#include <string>
#include <fstream>  
#include <sstream>   

using namespace std;

class UserManager {

private:
    vector<User> users;//users成员顺序表
    vector<Post> posts;//posts成员顺序表

    static UserManager* instance;
     // 私有构造函数
public:
    UserManager();
    
    static UserManager& getInstance();

    void insertUser(const User& user);

    bool deleteUser(const string& userId);

    User* getUser(const string& id);

    const vector<User> getAllUsers();

    void follow(const string& followerID,const string& followeeID);

    void unfollow(const string& followerID,const string& followeeID);

    vector<string> getFollowers(const string& userId);

    void loadFromFile(const string& filename);

    void saveToFile(const string& filename);

    // 获取所有帖子声明
    vector<Post> getAllPosts() const;
    //发帖函数声明
     void createPost(const Post& post);
     //帖子审核函数声明
    void moderatePosts();
    //发帖保存于文件声明
     void savePostsToFile(const std::string& filename);
    //从文件加载帖子声明
     void loadPostsFromFile(const std::string& filename);

};

#endif