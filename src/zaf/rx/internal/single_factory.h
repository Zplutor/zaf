#pragma once

#include <zaf/rx/single.h>

namespace zaf::rx::internal {

template<typename T>
class SingleFactory {
public:
    static Single<T> CreateSingle(std::shared_ptr<ObservableCore> core) noexcept {
        return Single<T>{ std::move(core) };
    }

    static SingleObserver<T> CreateSingleObserver(std::shared_ptr<ObserverCore> core) noexcept {
        return SingleObserver<T>{ std::move(core) };
    }
};

}