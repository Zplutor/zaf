#pragma once

#include <zaf/rx/internal/insider/insider.h>
#include <zaf/rx/observable.h>

namespace zaf::rx::internal {

class ObservableInsider {
public:
    template<typename T>
    static Observable<T> Create(std::shared_ptr<ObservableCore> core) noexcept {
        return Observable<T>{ std::move(core) };
    }

    template<typename T>
    static const std::shared_ptr<ObservableCore>& GetCore(
        const Observable<T>& observable) noexcept {

        return observable.Core();
    }
};


template<typename T>
class Insider<Observable<T>> {
public:
    using Type = ObservableInsider;
};

}