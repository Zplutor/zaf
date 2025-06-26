#pragma once

#include <functional>
#include <memory>

namespace zaf {
namespace rx::internal {
class InnerSubscription;
}

class Subscription {
public:
    Subscription();

    explicit Subscription(std::shared_ptr<rx::internal::InnerSubscription> inner) :
        inner_(std::move(inner)) { }

    Subscription(const Subscription&) = default;
    Subscription& operator=(const Subscription&) = default;

    Subscription(Subscription&& other);
    Subscription& operator=(Subscription&& other);

    void Unsubscribe();

    const std::shared_ptr<rx::internal::InnerSubscription>& Inner() const {
        return inner_;
    }

private:
    std::shared_ptr<rx::internal::InnerSubscription> inner_;
};


inline bool operator<(const Subscription& subscription1, const Subscription& subscription2) {
    return subscription1.Inner() < subscription2.Inner();
}

inline bool operator==(const Subscription& subscription1, const Subscription& subscription2) {
    return subscription1.Inner() == subscription2.Inner();
}

}