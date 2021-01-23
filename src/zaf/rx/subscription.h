#pragma once

#include <functional>
#include <memory>

namespace zaf {
namespace internal {
class InnerSubscription;
}

class Subscription {
public:
    Subscription();

    explicit Subscription(std::shared_ptr<internal::InnerSubscription> inner) :
        inner_(std::move(inner)) { }

    Subscription(const Subscription&) = default;
    Subscription& operator=(const Subscription&) = default;

    Subscription(Subscription&& other);
    Subscription& operator=(Subscription&& other);

    void Unsubscribe();

    const std::shared_ptr<internal::InnerSubscription>& GetInner() const {
        return inner_;
    }

private:
    std::shared_ptr<internal::InnerSubscription> inner_;
};


inline bool operator<(const Subscription& subscription1, const Subscription& subscription2) {
    return subscription1.GetInner() < subscription2.GetInner();
}

inline bool operator==(const Subscription& subscription1, const Subscription& subscription2) {
    return subscription1.GetInner() == subscription2.GetInner();
}

}