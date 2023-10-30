#pragma once

#include <cstddef>

class ReserveProxyObj {
public:
    explicit ReserveProxyObj(size_t capacity_to_reserve) : proxy_capacity_(capacity_to_reserve) {}

    size_t GetCapacity() {
        return proxy_capacity_;
    }

private:
    size_t proxy_capacity_;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}