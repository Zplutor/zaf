#pragma once

#include <string>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/subscription.h>

namespace zaf {
namespace internal {
class InnerSubscriptionHolder;
}

class SubscriptionHolder : NonCopyable {
public:
    class TagItem {
    public:
        std::string tag;
        Subscription subscription;
    };

public:
    SubscriptionHolder();
    ~SubscriptionHolder();

    SubscriptionHolder& operator+=(const Subscription& subscription);
    SubscriptionHolder& operator+=(const TagItem& item);
    SubscriptionHolder& operator-=(const std::string& tag);

    void Clear();

private:
    std::unique_ptr<internal::InnerSubscriptionHolder> inner_;
};


inline SubscriptionHolder::TagItem operator/(std::string tag, Subscription subscription) {
    return { std::move(tag), std::move(subscription) };
}

}