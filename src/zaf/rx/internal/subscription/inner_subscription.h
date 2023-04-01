#pragma once

#include <memory>

namespace zaf::internal {

class SubscriptionCore;

class InnerSubscription {
public:
    static std::shared_ptr<InnerSubscription> Empty();

public:
    InnerSubscription(std::shared_ptr<SubscriptionCore> core);
    ~InnerSubscription();

    InnerSubscription(const InnerSubscription&) = delete;
    InnerSubscription& operator=(const InnerSubscription&) = delete;

    void Unsubscribe();

    const std::shared_ptr<SubscriptionCore>& Core() {
        return core_;
    }

private:
    std::shared_ptr<SubscriptionCore> core_;
};

}