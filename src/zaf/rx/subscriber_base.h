#pragma once

#include <zaf/rx/internal/subscription/subscription_core.h>
#include <zaf/rx/subscription.h>

namespace zaf::rx {

template<
    template<typename> typename SUBSCRIBER,
    template<typename> typename OBSERVER, 
    typename T
>
class SubscriberBase : public OBSERVER<T> {
public:
    bool IsUnsubscribed() const noexcept {
        return subscription_core_->IsUnsubscribed();
    }

protected:
    SubscriberBase(
        std::shared_ptr<internal::ObserverCore> observer_core,
        std::shared_ptr<internal::SubscriptionCore> subscription_core) 
        :
        OBSERVER<T>(std::move(observer_core)),
        subscription_core_(std::move(subscription_core)) {

    }

private:
    std::shared_ptr<internal::SubscriptionCore> subscription_core_;
};

}