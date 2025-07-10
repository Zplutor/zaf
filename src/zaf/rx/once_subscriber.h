#pragma once

#include <zaf/rx/base_subscriber.h>
#include <zaf/rx/once_observer.h>

namespace zaf::rx {

template<typename T>
class OnceSubscriber : public BaseSubscriber<OnceSubscriber, OnceObserver, T> {
private:
    template<
        template<typename> typename OBSERVABLE,
        template<typename> typename OBSERVER,
        template<typename> typename SUBSCRIBER,
        typename K
    >
    friend class BaseObservable;

    using BaseSubscriber<OnceSubscriber, OnceObserver, T>::BaseSubscriber;
};

}