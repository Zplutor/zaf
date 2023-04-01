#pragma once

#include <functional>
#include <memory>

namespace zaf {
namespace internal {
class SubscriptionCore;
}

class Subscription {
public:
    /**
    Constructs an empty subscription.
    */
    Subscription();

    /**
    Constructs a subscription with specified core.
    */
    explicit Subscription(std::shared_ptr<internal::SubscriptionCore> core);

    Subscription(const Subscription&) = default;
    Subscription& operator=(const Subscription&) = default;

    Subscription(Subscription&& other);
    Subscription& operator=(Subscription&& other);

    void Unsubscribe();

    const std::shared_ptr<internal::SubscriptionCore>& Inner() const {
        return core_;
    }

private:
    std::shared_ptr<internal::SubscriptionCore> core_;
};


inline bool operator<(const Subscription& subscription1, const Subscription& subscription2) {
    return subscription1.Inner() < subscription2.Inner();
}

inline bool operator==(const Subscription& subscription1, const Subscription& subscription2) {
    return subscription1.Inner() == subscription2.Inner();
}

}