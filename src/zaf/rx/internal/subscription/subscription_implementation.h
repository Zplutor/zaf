#pragma once

#include <memory>

namespace zaf::internal {

class SubscriptionCore;

class SubscriptionImplementation {
public:
    static std::shared_ptr<SubscriptionImplementation> Empty();

public:
    SubscriptionImplementation(std::shared_ptr<SubscriptionCore> core);
    ~SubscriptionImplementation();

    SubscriptionImplementation(const SubscriptionImplementation&) = delete;
    SubscriptionImplementation& operator=(const SubscriptionImplementation&) = delete;

    void Unsubscribe();

    const std::shared_ptr<SubscriptionCore>& GetCore() {
        return core_;
    }

private:
    std::shared_ptr<SubscriptionCore> core_;
};

}