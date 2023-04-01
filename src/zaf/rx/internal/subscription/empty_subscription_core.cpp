#include <zaf/rx/internal/subscription/empty_subscription_core.h>

namespace zaf::internal {

std::shared_ptr<EmptySubscriptionCore> EmptySubscriptionCore::Instance() {

    static std::shared_ptr<EmptySubscriptionCore> instance{ new EmptySubscriptionCore() };
    return instance;
}


EmptySubscriptionCore::EmptySubscriptionCore() : SubscriptionCore(nullptr) {

}


void EmptySubscriptionCore::OnUnsubscribe() {
    //Nothing to do.
}

}