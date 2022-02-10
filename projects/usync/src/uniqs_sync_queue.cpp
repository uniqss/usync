#include "uniqs_sync_queue.h"


void UniqsSyncQueue::add(const char* rootdir, const char* filepath) {
    mtx_.lock();
    auto newval = std::make_pair<std::string, std::string>(rootdir, filepath);
    if (inqueue_.find(newval) == inqueue_.end()) {
        inqueue_.insert(newval);
        q_.push(newval);
    }
    mtx_.unlock();
}

int UniqsSyncQueue::foreach (int maxelement, std::function<void(const std::string&, const std::string&)> fn) {
    int i = 0;
    int ret = 0;
    mtx_.lock();
    int qsize = (int)q_.size();
    for (; i < maxelement && i < qsize; ++i) {
        auto element = q_.front();
        q_.pop();
        inqueue_.erase(element);
        fn(element.first, element.second);
    }
    ret = maxelement < qsize ? qsize - maxelement : 0;

    mtx_.unlock();
    return ret;
}
