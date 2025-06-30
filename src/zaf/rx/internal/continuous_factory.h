#pragma once

#include <zaf/rx/continuous.h>

namespace zaf::rx::internal {

template<typename T>
class ContinuousFactory {
public:
    static Continuous<T> Create(std::shared_ptr<ObservableCore> core) noexcept {
        return Continuous<T>{ std::move(core) };
    }
};

}