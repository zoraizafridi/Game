#pragma once
#include <unordered_map>
#include <functional>
#include <vector>
#include <string>
class EventBus {
public:
    using Callback = std::function<void(const std::string&, void*)>;
    void subscribe(const std::string& topic, Callback cb);
    void publish(const std::string& topic, void* payload = nullptr);
    void clearTopic(const std::string& topic);
private:
    std::unordered_map<std::string, std::vector<Callback>> topics_;
};