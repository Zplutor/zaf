#pragma once

#include <zaf/rx/internal/insider/insider.h>
#include <zaf/rx/observer.h>

namespace zaf::rx::internal {

class ObserverInsider {
public:
    template<typename T>
    static const std::shared_ptr<ObserverCore>& GetCore(const Observer<T>& observer) {
        return observer.Core();
    }
};

template<>
class Insider<Observer> {
public:
    using Type = ObserverInsider;
};

}