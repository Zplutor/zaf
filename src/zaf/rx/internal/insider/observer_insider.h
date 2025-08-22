#pragma once

#include <zaf/rx/internal/insider/insider.h>
#include <zaf/rx/observer.h>

namespace zaf::rx::internal {

class ObserverInsider {
public:
    template<typename T>
    static Observer<T> Create(std::shared_ptr<ObserverCore> core) noexcept {
        return Observer<T>{ std::move(core) };
    }

    template<typename T>
    static const std::shared_ptr<ObserverCore>& GetCore(const Observer<T>& observer) noexcept {
        return observer.Core();
    }
};

template<typename T>
class Insider<Observer<T>> {
public:
    using Type = ObserverInsider;
};

}