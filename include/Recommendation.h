
//
#ifndef RECOMMENDATION_H
#define RECOMMENDATION_H

#include <string>
#include <unordered_map>
#include <vector>
#include "Structure.h"
#include "UserManager.h"
using namespace std;

struct Recommendation {
    string creatorName;
    double score;
};

vector<User> RecommendCreators(const User& user, const std::vector<User>& allUsers);

vector<Post> recommendPostsByTags(const string& userId);

// 快速排序辅助函数声明
double calculatePostScore(const Post& post, const User& user);

int partitionPosts(vector<Post>& posts, int low, int high, const User& user);

void quickSortPosts(vector<Post>& posts, int low, int high, const User& user);

#endif