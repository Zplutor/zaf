#pragma once

#include <zaf/rx/internal/subscription/subscription_core.h>

namespace zaf::internal {

class EmptySubscriptionCore : public SubscriptionCore {
public:
    static std::shared_ptr<EmptySubscriptionCore> Instance();

protected:
    void OnUnsubscribe() override;

private:
    EmptySubscriptionCore();
};

}