#include "../include/Tags.h"
#include "../include/Structure.h"

void addTag(User& user, const string& tag) {
    for (const auto& existingTag : user.tags) {
        if (existingTag == tag) {
            return;  // 标签已存在，不重复添加
        }
    }
    user.tags.push_back(tag);
}
