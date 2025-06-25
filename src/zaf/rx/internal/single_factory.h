#pragma once

#include <zaf/rx/single.h>

namespace zaf::rx::internal {

template<typename T>
class SingleFactory {
public:
    static Single<T> Create(std::shared_ptr<zaf::internal::ObservableCore> core) noexcept {
        return Single<T>{ std::move(core) };
    }
};

}