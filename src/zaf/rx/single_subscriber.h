#pragma once

#include <zaf/rx/base_subscriber.h>
#include <zaf/rx/single_observer.h>

namespace zaf::rx {

template<typename T>
class SingleSubscriber : public BaseSubscriber<SingleSubscriber, SingleObserver, T> {
private:
    template<
        template<typename> typename OBSERVABLE,
        template<typename> typename OBSERVER,
        template<typename> typename SUBSCRIBER,
        typename K
    >
    friend class BaseObservable;

    using BaseSubscriber<SingleSubscriber, SingleObserver, T>::BaseSubscriber;
};

}