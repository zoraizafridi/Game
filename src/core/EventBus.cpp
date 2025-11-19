#include "../core/EventBus.hpp"
void EventBus::subscribe(const std::string& topic, Callback cb) {
    topics_[topic].push_back(cb);
}
void EventBus::publish(const std::string& topic, void* payload) {
    auto it = topics_.find(topic);
    if (it == topics_.end()) return;
    for (auto& cb : it->second)
        cb(topic, payload);
}
void EventBus::clearTopic(const std::string& topic) {
    topics_.erase(topic);
}