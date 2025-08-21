#pragma once

#include <zaf/rx/internal/insider/insider.h>
#include <zaf/rx/single_observer.h>

namespace zaf::rx::internal {

class SingleObserverInsider {
public:
    template<typename T>
    static const std::shared_ptr<ObserverCore>& GetCore(const SingleObserver<T>& observer) {
        return observer.Core();
    }
};

template<>
class Insider<SingleObserver> {
public:
    using Type = SingleObserverInsider;
};

}