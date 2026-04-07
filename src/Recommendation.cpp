#include "../include/Recommendation.h"
#include "../include/UserManager.h"
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

vector<User> RecommendCreators(const User& user, const std::vector<User>& allUsers) {
    vector<User> recommendations;
    
    // 只推荐创作者角色的用户
    for (const auto& creator : allUsers) {
        if (creator.role == Role::Creator) {
            double score = 0.0;
            // 计算兴趣匹配度
            for (const auto& userTag : user.tags) {
                for (const auto& creatorTag : creator.tags) {
                    if (userTag == creatorTag) {
                        score += 1.0;
                    }
                }
            }
            
            if (score > 0) {
                recommendations.push_back(creator);
            }
        }
    }
    
    // 使用冒泡排序按匹配度排序
    int n = recommendations.size();
    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; j++) {
            // 需要比较两个用户的匹配度
            int scoreJ = 0, scoreJ1 = 0;
            for (const auto& userTag : user.tags) {
                for (const auto& tag : recommendations[j].tags) {
                    if (userTag == tag) scoreJ++;
                }
                for (const auto& tag : recommendations[j+1].tags) {
                    if (userTag == tag) scoreJ1++;
                }
            }
            if (scoreJ < scoreJ1) {
                swap(recommendations[j], recommendations[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) {
            break;
        }
    }
    
    return recommendations;
}


//兴趣标签：兴趣内容推荐
double calculatePostScore(const Post& post, const User& user) {
    double score = 0.0;
    for (const auto& userTag : user.tags) {
        for (const auto& postTag : post.tags) {
            if (userTag == postTag) {
                score += 1.0;
            }
        }
    }
    return score;
}

// 快速排序的分区函数
int partitionPosts(vector<Post>& posts, int low, int high, const User& user) {
    Post pivot = posts[high];
    double pivotScore = calculatePostScore(pivot, user);
    
    int i = low - 1;
    
    for (int j = low; j < high; j++) {
        double currentScore = calculatePostScore(posts[j], user);
        if (currentScore >= pivotScore) {
            i++;
            swap(posts[i], posts[j]);
        }
    }
    swap(posts[i + 1], posts[high]);
    return i + 1;
}

// 函数4：快速排序主函数
void quickSortPosts(vector<Post>& posts, int low, int high, const User& user) {
    if (low < high) {
        int pi = partitionPosts(posts, low, high, user);
        
        quickSortPosts(posts, low, pi - 1, user);
        quickSortPosts(posts, pi + 1, high, user);
    }
}

// 基于标签的帖子推荐函数
vector<Post> recommendPostsByTags(const string& userId) {
    UserManager& userManager = UserManager::getInstance();//获取实例

    User* user = userManager.getUser(userId);
    if (!user) return vector<Post>();
    
    vector<Post> recommendedPosts;
    vector<Post> allPosts = userManager.getAllPosts();
    
    // 找到所有匹配度大于0的帖子
    for (const auto& post : allPosts) {
        double score = calculatePostScore(post, *user);
        if (score > 0) {
            recommendedPosts.push_back(post);
        }
    }
    
    // 使用快速排序按匹配度排序（降序）
    if (recommendedPosts.size() > 1) {
        quickSortPosts(recommendedPosts, 0, recommendedPosts.size() - 1, *user);
    }
    
    return recommendedPosts;
}