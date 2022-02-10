#include "uniqs_sync_queue.h"


void UniqsSyncQueue::add(const char* rootdir, const char* filepath) {
    mtx_.lock();
    auto newval = std::make_pair<std::string, std::string>(rootdir, filepath);
    if (inqueue_.find(newval) == inqueue_.end()) {
        inqueue_.emplace(newval);
        q_.emplace(newval);
    }
    mtx_.unlock();
}

void UniqsSyncQueue::foreach (int maxelement, std::function<void(const std::string&, const std::string&)> fn) {
    mtx_.lock();
    int qsize = (int)q_.size();
    for (int i = 0; i < maxelement && i < qsize; ++i) {
        auto element = q_.front();
        q_.pop();
        inqueue_.erase(element);
        fn(element.first, element.second);
    }

    mtx_.unlock();
}
