#pragma once

#include <zaf/rx/observable.h>

namespace zaf::rx::internal {

template<typename T>
class ObservableFactory {
public:
    static Observable<T> CreateObservable(std::shared_ptr<ObservableCore> core) noexcept {
        return Observable<T>{ std::move(core) };
    }

    static Observer<T> CreateObserver(std::shared_ptr<ObserverCore> core) noexcept {
        return Observer<T>{ std::move(core) };
    }
};

}