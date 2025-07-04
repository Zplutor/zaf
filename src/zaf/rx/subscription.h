#pragma once

#include <functional>
#include <memory>

namespace zaf::rx::internal {
class SubscriptionCore;
}

namespace zaf {

class Subscription {
public:
    Subscription() = default;
    explicit Subscription(std::shared_ptr<rx::internal::SubscriptionCore> core);

    Subscription(const Subscription&) = default;
    Subscription& operator=(const Subscription&) = default;

    Subscription(Subscription&& other) = default;
    Subscription& operator=(Subscription&& other) = default;

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