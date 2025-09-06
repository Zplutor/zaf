#pragma once

/**
@file
    Defines the `zaf::rx::SingleSubscriber<>` class template.
*/

#include <zaf/rx/single_observer.h>
#include <zaf/rx/subscriber_base.h>

namespace zaf::rx {

/**
Represents a subscriber that receives emissions from a custom `zaf::rx::Single<>`.

@tparam T
    The type of the item received by the subscriber.
*/
template<typename T>
class SingleSubscriber : public SubscriberBase<SingleSubscriber, SingleObserver, T> {
private:
    template<
        template<typename> typename OBSERVABLE,
        template<typename> typename OBSERVER,
        template<typename> typename SUBSCRIBER,
        typename K
    >
    friend class ObservableBase;

    using SubscriberBase<SingleSubscriber, SingleObserver, T>::SubscriberBase;
};

}
