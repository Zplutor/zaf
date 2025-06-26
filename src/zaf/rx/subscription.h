#pragma once

#include <functional>
#include <memory>

namespace zaf {
namespace rx::internal {
class SubscriptionCore;
}

class Subscription {
public:
    Subscription();

    explicit Subscription(std::shared_ptr<rx::internal::SubscriptionCore> core) :
        core_(std::move(core)) { }

    Subscription(const Subscription&) = default;
    Subscription& operator=(const Subscription&) = default;

    Subscription(Subscription&& other);
    Subscription& operator=(Subscription&& other);

    void Unsubscribe();

    const std::shared_ptr<rx::internal::SubscriptionCore>& Core() const {
        return core_;
    }

private:
    std::shared_ptr<rx::internal::SubscriptionCore> core_;
};


inline bool operator<(const Subscription& subscription1, const Subscription& subscription2) {
    return subscription1.Core() < subscription2.Core();
}

inline bool operator==(const Subscription& subscription1, const Subscription& subscription2) {
    return subscription1.Core() == subscription2.Core();
}

}