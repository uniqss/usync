#pragma once

#include <atomic>
#include <mutex>

#include <unordered_set>
#include <string>

extern std::atomic_bool g_working;
extern std::atomic_int g_waitgroupTerminating;

struct StringPairHash {
    std::size_t operator()(const std::pair<std::string, std::string>& key) const {
        std::string combined = key.first + key.second;
        return std::hash<std::string>()(combined);
    }
};

extern std::unordered_set<std::pair<std::string, std::string>, StringPairHash> g_listNewOrChanged;
extern std::unordered_set<std::pair<std::string, std::string>, StringPairHash> g_listDeleted;
extern std::mutex g_listNewOrChangedMtx;
extern std::mutex g_listDeletedMtx;


// remember to change this on other platforms. g++ can't detech win.
#if 1
#define _WIN64
#endif
