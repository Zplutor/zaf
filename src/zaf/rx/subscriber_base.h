#pragma once

namespace zaf::rx {

class Disposable;

template<
    template<typename> typename SUBSCRIBER,
    template<typename> typename OBSERVER, 
    typename T
>
class SubscriberBase : public OBSERVER<T> {
public:
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