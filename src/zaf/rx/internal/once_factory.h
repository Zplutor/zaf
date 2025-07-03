#pragma once

#include <zaf/rx/once.h>

namespace zaf::rx::internal {

template<typename T>
class OnceFactory {
public:
    static Once<T> CreateOnce(std::shared_ptr<ObservableCore> core) noexcept {
        return Once<T>{ std::move(core) };
    }

    static OnceObserver<T> CreateOnceObserver(std::shared_ptr<ObserverCore> core) noexcept {
        return OnceObserver<T>{ std::move(core) };
    }
};

}