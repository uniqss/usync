#pragma once

#include <queue>
#include <unordered_set>
#include <string>
#include <mutex>
#include <functional>

struct StringPairHash {
    std::size_t operator()(const std::pair<std::string, std::string>& key) const {
        std::string combined = key.first + key.second;
        return std::hash<std::string>()(combined);
    }
};

class UniqsSyncQueue {
    std::mutex mtx_;
    std::queue<std::pair<std::string, std::string>> q_;
    std::unordered_set<std::pair<std::string, std::string>, StringPairHash> inqueue_;

   public:
    void add(const char* rootdir, const char* filepath);
    void foreach (int maxelement, std::function<void(const std::string&, const std::string&)> fn);
};
