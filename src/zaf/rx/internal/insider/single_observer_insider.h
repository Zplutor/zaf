#pragma once

#include <zaf/rx/internal/insider/insider.h>
#include <zaf/rx/single_observer.h>

namespace zaf::rx::internal {

class SingleObserverInsider {
public:
    template<typename T>
    static SingleObserver<T> Create(std::shared_ptr<ObserverCore> core) noexcept {
        return SingleObserver<T>{ std::move(core) };
    }

    template<typename T>
    static const std::shared_ptr<ObserverCore>& GetCore(
        const SingleObserver<T>& observer) noexcept {

        return observer.Core();
    }
};

template<typename T>
class Insider<SingleObserver<T>> {
public:
    using Type = SingleObserverInsider;
};

}