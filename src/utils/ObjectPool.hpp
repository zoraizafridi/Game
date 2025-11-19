#pragma once
#include <vector>
#include <functional>
template<typename T>
class ObjectPool {
public:
    ObjectPool(size_t size = 128) {
        items_.resize(size);
        active_.resize(size, false);
    }
    int allocate() {
        for (size_t i = 0; i < items_.size(); i++) {
            if (!active_[i]) {
                active_[i] = true;
                return static_cast<int>(i);
            }
        }
        return -1;
    }
    void free(int index) {
        if (index >= 0 && index < static_cast<int>(active_.size())) {
            active_[index] = false;
        }
    }
    // Return non-const references
    T& get(int index) { 
        return items_[index]; 
    }
    const T& get(int index) const { 
        return items_[index]; 
    }
    // Return non-const references to the underlying vectors
    std::vector<T>& raw() { 
        return items_; 
    }
    const std::vector<T>& raw() const { 
        return items_; 
    }
    std::vector<bool>& activeFlags() { 
        return active_; 
    }
    const std::vector<bool>& activeFlags() const { 
        return active_; 
    }
private:
    std::vector<T> items_;
    std::vector<bool> active_;
};