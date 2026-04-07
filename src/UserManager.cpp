#include "../include/UserManager.h"
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>


UserManager* UserManager::instance = nullptr;


UserManager::UserManager() {
}

UserManager& UserManager::getInstance() {
    static UserManager instance;
    return instance;
}

void UserManager::insertUser(const User& user) {
    auto it = users.begin();
    while (it != users.end() && it->id < user.id) {
        ++it;
    }

    if (it != users.end() && it->id == user.id) {
        *it = user;
    }
    else {
        users.insert(it, user);
    }
}

bool UserManager::deleteUser(const string& userId) {
    for (auto it = users.begin(); it != users.end(); ++it) {
        if (it->id == userId) {
            users.erase(it);
            return true;
        }
    }
    return false;
}

User* UserManager::getUser(const string& id) {
    int left = 0;
    int right = users.size();

    while (left < right) {
        int mid = left + (right - left) / 2;

        if (users[mid].id == id) {
            return &users[mid];
        }
        else if (users[mid].id < id) {
            left = mid + 1;
        }
        else {
            right = mid;
        }
    }

    return nullptr;
}

const vector<User> UserManager::getAllUsers() {
    return users;
}

vector<Post> UserManager::getAllPosts() const {
    return posts;
}

void UserManager::follow(const string& followerID, const string& followeeID) {
    User* follower = getUser(followerID);
    User* followee = getUser(followeeID);

    if (follower && followee) {
        if (find(follower->following.begin(), follower->following.end(), followeeID)
            == follower->following.end()) {
            follower->following.push_back(followeeID);
        }

        if (find(followee->followers.begin(), followee->followers.end(), followerID)
            == followee->followers.end()) {
            followee->followers.push_back(followerID);
        }
    }
}

void UserManager::unfollow(const string& followerID, const string& followeeID) {
    User* follower = getUser(followerID);
    User* followee = getUser(followeeID);

    if (follower && followee) {
        auto& following = follower->following;
        following.erase(remove(following.begin(), following.end(), followeeID), following.end());

        auto& followers = followee->followers;
        followers.erase(remove(followers.begin(), followers.end(), followerID), followers.end());
    }
}

vector<string> UserManager::getFollowers(const string& userId) {
    User* user = getUser(userId);
    if (user) {
        return user->followers;
    }
    return vector<string>();
}

void UserManager::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string id, name, password, roleStr;
        vector<string> tags;
        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, password, ',');
        getline(ss, roleStr, ',');
        for (string tag; getline(ss, tag, ',');) {
            tags.push_back(tag);
        }
        if (id.empty() || name.empty()) {
            continue;
        }
        Role role;
        if (roleStr == "Admin") role = Role::Admin;
        else if (roleStr == "Creator") role = Role::Creator;
        else role = Role::Subscriber;
        User user(id, name, password, role, tags);
        users.push_back(user);
    }
    file.close();
}

void UserManager::saveToFile(const string& filename) {
    ofstream file(filename);
    for (const auto& user : users) {
        string roleStr;
        if (user.role == Role::Admin) roleStr = "Admin";
        else if (user.role == Role::Creator) roleStr = "Creator";
        else roleStr = "Subscriber";
        file << user.id << "," << user.name << "," << user.password << "," << roleStr ;
        for (const auto& tag : user.tags) {
            file << "," << tag;
        }
        file << endl;
    }
    file.close();
}

void UserManager::createPost(const Post& post) {
    posts.push_back(post);
    cout << "帖子发布成功。" << endl;
}

//审核
void UserManager::moderatePosts() {
    cout << "\n--- 开始审核 ---\n";
    bool hasPending = false;
    const vector<string> forbiddenWords = { "垃圾", "骗子", "违法" };

    for (auto& post : posts) {
        if (post.status == "pending") {
            hasPending = true;
            cout << "审核帖子 (ID: " << post.id << "): " << post.content << endl;

            bool isApproved = true;
            for (const auto& word : forbiddenWords) {
                if (post.content.find(word) != string::npos) {
                    isApproved = false;
                    break;
                }
            }

            post.status = isApproved ? "approved" : "rejected";
            cout << "结果: " << (isApproved ? "通过" : "拒绝") << "\n--------------------" << endl;
        }
    }

    if (!hasPending) cout << "没有待审核帖子。" << endl;
    cout << "--- 审核结束 ---\n" << endl;
}

void UserManager::savePostsToFile(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "错误: 无法写入文件 " << filename << endl;
        return;
    }
    for (const auto& p : posts) {
        file << p.id << ","
             << p.postId << ","
             << p.creatorId << ","
             << p.content << ","
             << p.status <<","
             << p.timestamp << "\n";
    }
    file.close();
    cout << "帖子已保存到 " << filename << endl;
}


void UserManager::loadPostsFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "警告: 无法读取文件 " << filename << endl;
        return;
    }
    posts.clear();
    string line, token;
    while (getline(file, line)) {

        if (line.empty()) continue;

        stringstream iss(line);
        Post p;
        getline(iss, token, ','); p.id = stoi(token);
        getline(iss, p.postId, ',');
        getline(iss, p.creatorId, ',');
        getline(iss, p.content, ',');
        getline(iss, p.status, ',');
        getline(iss, token, ','); p.timestamp = stoll(token);

        // string tag;
        // p.tags.clear();
        //   while(getline(iss, tag, ',')) {
        //     if (!tag.empty()) {
        //         p.tags.push_back(tag);
        //     }
        // }

        posts.push_back(p);
    }
    file.close();
    cout << "帖子已从 " << filename << " 加载。" << endl;
}
