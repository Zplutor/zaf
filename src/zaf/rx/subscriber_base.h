#pragma once

/**
@file
    Defines the `zaf::rx::SubscriberBase<>` class template.
*/

namespace zaf::rx {

class Disposable;

/**
A base class template for subscriber types, providing common subscriber functionalities.

@tparam SUBSCRIBER
    The subscriber type that derives from this base class template.

@tparam OBSERVER
    The observer type used for subscribing to the observable.

@tparam T
    The type of items received by the subscriber.

@details
    A subscriber is essentially an observer that also holds a subscription to the observable. It is
    used in a custom observable's producer function to send emissions to the observers and manage
    the subscription lifecycle.

@see zaf::rx::SingleSubscriber<>
@see zaf::rx::Subscriber<>
*/
template<
    template<typename> typename SUBSCRIBER,
    template<typename> typename OBSERVER, 
    typename T
>
class SubscriberBase : public OBSERVER<T> {
public:
    /**
    Indicates whether the subscription held by the current subscriber has been disposed.

    @return
        `true` if the subscription has been disposed; otherwise, `false`.
    */
    bool IsDisposed() const noexcept {
        return subscription_core_->IsDisposed();
    }

protected:
    SubscriberBase(
        std::shared_ptr<internal::ObserverCore> observer_core,
        std::shared_ptr<Disposable> subscription_core) 
        :
        OBSERVER<T>(std::move(observer_core)),
        subscription_core_(std::move(subscription_core)) {

    }

private:
    std::shared_ptr<Disposable> subscription_core_;
};

}
