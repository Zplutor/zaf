#pragma once

#include <zaf/rx/continuous.h>

namespace zaf::rx::internal {

template<typename T>
class ContinuousFactory {
public:
    static Continuous<T> CreateContinuous(std::shared_ptr<ObservableCore> core) noexcept {
        return Continuous<T>{ std::move(core) };
    }

    static ContinuousObserver<T> CreateContinuousObserver(
        std::shared_ptr<ObserverCore> core) noexcept {

        return ContinuousObserver<T>{ std::move(core) };
    }
};

}