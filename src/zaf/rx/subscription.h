#pragma once

#include <functional>
#include <memory>

namespace zaf::rx {

class Disposable;

class Subscription {
public:
    Subscription() = default;
    explicit Subscription(std::shared_ptr<Disposable> core) noexcept;
    ~Subscription();

    Subscription(const Subscription&) = default;
    Subscription& operator=(const Subscription&) = default;

    Subscription(Subscription&& other) = default;
    Subscription& operator=(Subscription&& other) = default;

    void Dispose() noexcept;

    bool IsDisposed() const noexcept;

    const std::shared_ptr<Disposable>& Core() const noexcept {
        return core_;
    }

private:
    std::shared_ptr<Disposable> core_;
};


inline bool operator<(const Subscription& subscription1, const Subscription& subscription2) {
    return subscription1.Core() < subscription2.Core();
}

inline bool operator==(const Subscription& subscription1, const Subscription& subscription2) {
    return subscription1.Core() == subscription2.Core();
}

}