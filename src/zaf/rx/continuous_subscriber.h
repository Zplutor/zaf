#pragma once

#include <zaf/rx/base_subscriber.h>
#include <zaf/rx/continuous_observer.h>

namespace zaf::rx {

template<typename T>
class ContinuousSubscriber : public BaseSubscriber<ContinuousSubscriber, ContinuousObserver, T> {
private:
    template<
        template<typename> typename OBSERVABLE,
        template<typename> typename OBSERVER,
        template<typename> typename SUBSCRIBER,
        typename K
    >
    friend class BaseObservable;

    using BaseSubscriber<ContinuousSubscriber, ContinuousObserver, T>::BaseSubscriber;
};

}