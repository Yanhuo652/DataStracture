#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <fstream>

#include "../include/UserManager.h"
#include "../include/Tags.h"
#include "../include/Recommendation.h"  
#include "../include/Structure.h"

using namespace std;


// 订阅者菜单
void subscriberMenu(UserManager& userManager, const string& userId) {
    int choice;
    while (true) {
        cout << "\n===============订阅者菜单================" << endl;
        cout <<"\t"<<"1. 查看推荐创作者" << endl;
        cout <<"\t"<<"2. 关注用户" << endl;
        cout <<"\t"<<"3. 查看关注列表" << endl;
        cout <<"\t"<<"4. 注销账号" << endl;
        cout <<"\t"<<"5. 修改兴趣标签" << endl;
        cout <<"\t"<<"6. 查看兴趣推送内容" << endl;
        cout <<"\t"<<"0. 退出登录" << endl;
        cout << "=========================================" << endl;
        cout <<"\t"<<"请输入操作编号: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            User* user = userManager.getUser(userId);
            if (user) {
                auto recommendations = RecommendCreators(*user, userManager.getAllUsers());
                cout << "\n【推荐创作者】" << endl;
                if (recommendations.empty()) {
                    cout <<"\t"<<"暂无推荐创作者!" << endl;
                    break;
                }
                for (const auto& u : recommendations) {
                    cout <<"\t"<<"ID: " << u.id << "  姓名: " << u.name << endl;
                }
            }
            break;
        }
        case 2: {
            string followeeId;
            cout << "\n""\t"<<"请输入要关注的用户ID: ";
            cin >> followeeId;
            userManager.follow(userId, followeeId);
            cout <<"\t"<<"关注成功!" << endl;
            break;
        }
        case 3: {
            User* user = userManager.getUser(userId);
            if (user) {
                cout << "\n""\t"<<"【我的关注列表】" << endl;
                if (user->following.empty()) {
                    cout <<"\t"<<"您尚未关注任何人!" << endl;
                    break;
                }
                for (const auto& id : user->following) {
                    cout <<"\t"<<id << endl;
                }
            }
            break;
        }
        case 4: {
            if (userManager.deleteUser(userId)) {
                cout << "\n""\t"<<"注销成功,即将返回主菜单!" << endl;
                return;
            }
            else {
                cout << "\n""\t"<<"注销失败!" << endl;
            }
            break;
        }
        case 5: {
            User* user = userManager.getUser(userId);
            if (user) {
                cout <<"\n""\t"<<"当前兴趣标签: ";
                for (const auto& tag : user->tags) {
                    cout <<"\t"<<tag << " ";
                }
                cout << endl;

                cout <<"\t"<<"请输入新标签数量: ";
                int tagCount;
                cin >> tagCount;

                user->tags.clear();
                for (int i = 0; i < tagCount; ++i) {
                    string tag;
                    cout <<"\t"<<"输入第" << i + 1 << "个标签: ";
                    cin >> tag;
                    addTag(*user, tag);
                }
                cout <<"\t"<<"兴趣标签修改成功!" << endl;
            }
            break;
        }
        case 6: {
            User* user = userManager.getUser(userId);
            if (user) {
                cout << "\n【兴趣推送内容】" << endl;
                vector<Post> recommendedPosts = recommendPostsByTags(userId);
                if (recommendedPosts.empty()) {
                    cout <<"\t"<<"暂无匹配的推送内容!" << endl;
                    break;
                }
                for (const auto& post : recommendedPosts) {
                    cout <<"\t"<<"创作者ID: " << post.creatorId << endl;
                    cout <<"\t"<<"帖子内容: " << post.content << endl;
                    cout <<"\t"<<"帖子标签: ";
                    for (const auto& tag : post.tags) {
                        cout << tag << " ";
                    }
                    cout << "\n------------------------" << endl;
                }
            }
            break;
        }
        case 0: {
            cout << "\n退出登录成功!" << endl;
            return;
        }
        default:
            cout << "\n无效操作编号,请重新输入!" << endl;
            break;
        }
    }
}

// 创作者菜单
void creatorMenu(UserManager& userManager, const string& userId) {
    int choice;
    while (true) {
        cout << "\n===============创作者菜单================" << endl;
        cout <<"\t"<<"1. 发帖" << endl;
        cout <<"\t"<<"2. 查看订阅者列表" << endl;
        cout <<"\t"<<"3. 注销账号" << endl;
        cout <<"\t"<<"4. 修改兴趣标签" << endl;
        cout <<"\t"<<"0. 退出登录" << endl;
        cout << "=========================================" << endl;
        cout <<"\t"<<"请输入操作编号: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            Post newPost;
            newPost.creatorId = userId;//业务
            
            cin.ignore();// 在读取帖子内容前，清除之前输入的换行符
            cout << "\n""\t"<<"请输入帖子内容: ";
            getline(cin, newPost.content);

             if (newPost.content.empty()) {
                cout <<"\t"<<"帖子内容不能为空!" << endl;
                break;
            }

            cout <<"\t"<<"请输入帖子标签数量: ";
            int tagCount;
            cin >> tagCount;
           

             if (tagCount <= 0) {
                cout <<"\t"<<"标签数量必须大于0!" << endl;
                break;
            }


            for (int i = 0; i < tagCount; ++i) {
                string tag;
                cout <<"\t"<<"输入第" << i + 1 << "个标签: ";
                cin >> tag;

                if (!tag.empty()) {
                    newPost.tags.push_back(tag);
                }
            }

            // 生成唯一帖子ID和时间戳
            time_t now = time(nullptr);
            newPost.id = static_cast<int>(now);
            newPost.postId = newPost.creatorId + "_" + to_string(now);
            newPost.timestamp = now;
            newPost.status = "pending";

             

            // 添加并保存帖子
            userManager.createPost(newPost);
            userManager.savePostsToFile("posts.txt");
            cout << "\n发帖成功!帖子ID: " << newPost.postId << endl;
            break;
        }
        case 2: {
            auto followers = userManager.getFollowers(userId);
            cout << "\n【我的订阅者列表】" << endl;
            if (followers.empty()) {
                cout <<"\t"<<"您暂无订阅者!" << endl;
                break;
            }
            for (const auto& f : followers) {
                cout << f << endl;
            }
            break;
        }
        case 3: {
            if (userManager.deleteUser(userId)) {
                cout << "\n""\t"<<"注销成功,即将返回主菜单!" << endl;
                return;
            }
            else {
                cout <<"\t"<<"\n注销失败!" << endl;
            }
            break;
        }
        case 4: {
            User* user = userManager.getUser(userId);
            if (user) {
                cout << "\n""\t"<<"当前兴趣标签: ";
                for (const auto& tag : user->tags) {
                    cout << tag << " ";
                }
                cout << endl;

                cout <<"\t"<<"请输入新标签数量: ";
                int tagCount;
                cin >> tagCount;

                user->tags.clear();
                for (int i = 0; i < tagCount; ++i) {
                    string tag;
                    cout <<"\t"<<"输入第" << i + 1 << "个标签: ";
                    cin >> tag;
                    addTag(*user, tag);
                }
                cout <<"\t"<<"兴趣标签修改成功!" << endl;
            }
            break;
        }
        case 0: {
            cout << "\n""\t"<<"退出登录成功!" << endl;
            return;
        }
        default:
            cout << "\n""\t"<<"无效操作编号,请重新输入!" << endl;
            break;
        }
    }
}

// 管理员菜单
void adminMenu(UserManager& userManager, const string& userId) {
    int choice;
    while (true) {
        cout << "\n===============管理员菜单================" << endl;
        cout <<"\t"<<"1. 审核用户ID" << endl;
        cout <<"\t"<<"2. 审核创作者内容" << endl;
        cout <<"\t"<<"3. 删除用户" << endl;
        cout <<"\t"<<"4. 修改用户密码" << endl;
        cout <<"\t"<<"0. 退出登录" << endl;
        cout << "=========================================" << endl;
        cout <<"\t"<<"请输入操作编号: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            cout << "\n【所有用户ID及角色】" << endl;
            auto allUsers = userManager.getAllUsers();
            if (allUsers.empty()) {
                cout <<"\t"<<"暂无用户数据!" << endl;
                break;
            }
            for (const auto& u : allUsers) {
                string roleName;
                if (u.role == Role::Admin) roleName = "管理员";
                else if (u.role == Role::Creator) roleName = "创作者";
                else roleName = "订阅者";
                cout <<"\t"<<"用户ID: " << u.id << "  角色: " << roleName << endl;
            }
            break;
        }
        case 2: {
            cout << "\n【创作者内容审核】" << endl;
            vector<Post> allPosts = userManager.getAllPosts();

            if (allPosts.empty()) {
                cout <<"\t"<<"暂无内容需要审核!" << endl;
                break;
            }   
    
            cout <<"\t"<<"当前所有帖子：" << endl;
            int pendingCount = 0;
            for (const auto& post : allPosts) {
                cout <<"\t"<<"帖子ID: " << post.postId << endl;
                cout <<"\t"<<"创作者ID: " << post.creatorId << endl;
                cout <<"\t"<<"帖子内容: " << post.content << endl;
                cout <<"\t"<<"状态: " << post.status << endl;
                cout <<"\t"<<"时间戳: " << post.timestamp << endl;
                cout <<"\t"<<"帖子标签: ";
                for (const auto& tag : post.tags) {
                    cout << tag << " ";
                 }
                    cout << "\n------------------------" << endl;
        
                    if (post.status == "pending") pendingCount++;
            }
    
            if (pendingCount == 0) {
                cout <<"\t"<<"没有待审核的帖子。" << endl;
            } else {
                cout << "\n""\t"<<"开始审核待审核的帖子..." << endl;
            // 执行实际审核
                userManager.moderatePosts();
                cout <<"\t"<<"审核完成！" << endl;
        
            // 保存审核结果
                userManager.savePostsToFile("posts.txt");
            }
            break;
        }
        case 4: {
            string modId, newPass;
            cout << "\n""\t"<<"请输入要修改密码的用户ID: ";
            cin >> modId;
            cout <<"\t"<<"请输入新密码: ";
            cin >> newPass;

            User* user = userManager.getUser(modId);
            if (user) {
                user->password = newPass;
                cout <<"\t"<<"用户密码修改成功!" << endl;
            }
            else {
                cout <<"\t"<<"用户不存在,修改失败!" << endl;
            }
            break;
        }
        case 0: {
            cout << "\n""\t"<<"退出登录成功!" << endl;
            return;
        }
        default:
            cout << "\n""\t"<<"无效操作编号,请重新输入!" << endl;
            break;
        }
    }
}

// 主函数
int main() {
    UserManager& userManager = UserManager::getInstance();
    // 加载用户和帖子数据
    userManager.loadFromFile("user.txt");
    userManager.loadPostsFromFile("posts.txt");

    int choice;
    while (true) {
        cout << "\n===============系统主菜单================" << endl;
        cout << "\t"<<"1. 注册用户并设置兴趣标签" << endl;
        cout << "\t"<<"2. 登录" << endl;
        cout << "\t"<<"0. 退出系统" << endl;
        cout << "=========================================" << endl;
        cout << "\t"<<"请输入操作编号: ";
        cin >> choice;
        cout << endl;

        switch (choice) {
        case 1: {
            string id, name, password, roleStr;
            vector<string> tags;
            cout << "【用户注册】" << endl;
            cout <<"\t"<<"请输入用户ID: ";
            cin >> id;
            cout <<"\t"<<"请输入用户名: ";
            cin >> name;
            cout <<"\t"<<"请输入密码: ";
            cin >> password;
            cout <<"\t"<<"请输入角色(Subscriber/Creator/Admin): ";
            cin >> roleStr;

            Role role;
            if (roleStr == "Admin") {
                role = Role::Admin;
            }
            else if (roleStr == "Creator") {
                role = Role::Creator;
            }
            else if (roleStr == "Subscriber") {
                role = Role::Subscriber;
            }
            else {
                cout <<"\t"<<"无效角色类型,注册失败!" << endl;
                break;
            }
  
            
            
            User newUser(id, name, password, role, tags);


            if (userManager.getUser(id) == nullptr) {
            cout <<"\t"<<"请输入兴趣标签数量: ";
            int tagCount;
            cin >> tagCount;
            for (int i = 0; i < tagCount; ++i) {
                string tag;
                cout <<"\t"<<"输入第" << i + 1 << "个标签: ";
                cin >> tag;
                addTag(newUser, tag);
            }

            userManager.insertUser(newUser);
            cout <<"\t"<<"用户注册成功!" << endl;
            break;
            }else{
                cout <<"\t"<<"用户ID已存在,注册失败!" << endl;}
        }
        case 2: {// 验证登录信息
            string id, password;
            int roleChoice;
            cout <<"【用户登录】" << endl;
            cout <<"\t"<<"请输入用户ID: ";
            cin >> id;
            cout <<"\t"<<"请输入密码: ";
            cin >> password;
            User* user = userManager.getUser(id);
                if (user && user->password == password) {
                    cout <<"\t"<<"登录成功!\n";
                    if (user->role == Role::Subscriber) {
                        subscriberMenu(userManager, id);
                    } else if (user->role == Role::Creator) {
                        creatorMenu(userManager, id);
                    } else if (user->role == Role::Admin) {
                        adminMenu(userManager, id);
                    }
                } else {
                if (!user) {
                    cout <<"\t"<<"用户不存在,登录失败!" << endl;
                }
                else if (user->password != password) {
                    cout <<"\t"<<"密码错误,登录失败!" << endl;
                }
                else {
                    cout <<"\t"<<"角色不匹配,登录失败!" << endl;
                }
            }
            break;
        }
        case 0: {
            // 保存数据并退出
            userManager.saveToFile("user.txt");
            userManager.savePostsToFile("posts.txt");
            cout << "感谢使用系统,再见!" << endl;
            return 0;
        }
        default:
            cout << "无效操作编号,请重新输入!" << endl;
            break;
        }
    }
    return 0;
}

