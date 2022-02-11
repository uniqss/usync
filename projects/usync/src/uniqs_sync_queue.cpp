#include "uniqs_sync_queue.h"


void UniqsSyncQueue::add(const char* rootdir, const char* filepath) {
    std::lock_guard<std::mutex> _(mtx_);
    auto newval = std::make_pair<std::string, std::string>(rootdir, filepath);
    if (inqueue_.find(newval) == inqueue_.end()) {
        inqueue_.insert(newval);
        q_.push(newval);
    }
}

int UniqsSyncQueue::foreach (int maxelement, std::function<void(const std::string&, const std::string&)> fn) {
    std::lock_guard<std::mutex> _(mtx_);
    int i = 0;
    int ret = 0;
    int qsize = (int)q_.size();
    for (; i < maxelement && i < qsize; ++i) {
        auto element = q_.front();
        q_.pop();
        inqueue_.erase(element);
        fn(element.first, element.second);
    }
    ret = maxelement < qsize ? qsize - maxelement : 0;

    return ret;
}
