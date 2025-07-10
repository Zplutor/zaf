#pragma once

#include <zaf/rx/base_subscriber.h>
#include <zaf/rx/observer.h>

namespace zaf::rx {

template<typename T>
class Subscriber : public BaseSubscriber<Subscriber, Observer, T> {
private:
    template<
        template<typename> typename OBSERVABLE,
        template<typename> typename OBSERVER,
        template<typename> typename SUBSCRIBER,
        typename K
    >
    friend class BaseObservable;

    using BaseSubscriber<Subscriber, Observer, T>::BaseSubscriber;
};

}
