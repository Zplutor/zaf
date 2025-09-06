#pragma once

/**
@file
    Defines the `zaf::rx::Subscriber<>` class template.
*/

#include <zaf/rx/observer.h>
#include <zaf/rx/subscriber_base.h>

namespace zaf::rx {

/**
Represents a subscriber that receives emissions from a custom `zaf::rx::Observable<>`.

@tparam T
    The type of the items received by the subscriber.
*/
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
