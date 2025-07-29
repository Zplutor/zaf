#pragma once

#include <zaf/rx/observer.h>
#include <zaf/rx/subscriber_base.h>

namespace zaf::rx {

template<typename T>
class Subscriber : public SubscriberBase<Subscriber, Observer, T> {
private:
    template<
        template<typename> typename OBSERVABLE,
        template<typename> typename OBSERVER,
        template<typename> typename SUBSCRIBER,
        typename K
    >
    friend class ObservableBase;

    using SubscriberBase<Subscriber, Observer, T>::SubscriberBase;
};

}
