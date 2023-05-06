#pragma once

#include <string>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/subscription.h>

namespace zaf {
namespace internal {
class InnerSubscriptionSet;
}

class SubscriptionSet : NonCopyableNonMovable {
public:
    class TagItem {
    public:
        std::string tag;
        Subscription subscription;
    };

public:
    SubscriptionSet();
    ~SubscriptionSet();

    SubscriptionSet& operator+=(const Subscription& subscription);
    SubscriptionSet& operator+=(const TagItem& item);
    SubscriptionSet& operator-=(const std::string& tag);

    void Clear();

    std::size_t Count() const;

private:
    std::unique_ptr<internal::InnerSubscriptionSet> inner_;
};


inline SubscriptionSet::TagItem operator/(std::string tag, Subscription subscription) {
    return { std::move(tag), std::move(subscription) };
}

}