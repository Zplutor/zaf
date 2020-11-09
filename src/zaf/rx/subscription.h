#pragma once

#include <functional>
#include <memory>

namespace zaf {
namespace internal {
class SubscriptionImplementation;
}

class Subscription {
public:
    Subscription();

    explicit Subscription(std::shared_ptr<internal::SubscriptionImplementation> implementation) :
        implementation_(std::move(implementation)) { }

    Subscription(const Subscription&) = default;
    Subscription& operator=(const Subscription&) = default;

    Subscription(Subscription&& other);
    Subscription& operator=(Subscription&& other);

    void Unsubscribe();

    const std::shared_ptr<internal::SubscriptionImplementation>& GetImplementation() const {
        return implementation_;
    }

private:
    std::shared_ptr<internal::SubscriptionImplementation> implementation_;
};


inline bool operator<(const Subscription& subscription1, const Subscription& subscription2) {
    return subscription1.GetImplementation() < subscription2.GetImplementation();
}

inline bool operator==(const Subscription& subscription1, const Subscription& subscription2) {
    return subscription1.GetImplementation() == subscription2.GetImplementation();
}

}