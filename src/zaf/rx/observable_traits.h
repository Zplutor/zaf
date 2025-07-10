#pragma once

#include <zaf/rx/observer.h>
#include <zaf/rx/subscriber.h>

namespace zaf::rx {

struct ObservableTraits {

    template<typename T>
    using ObserverType = Observer<T>;

    template<typename T>
    using SubscriberType = Subscriber<T>;
};

}