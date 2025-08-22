#pragma once

#include <zaf/rx/single.h>

namespace zaf::rx::internal {

class SingleInsider {
public:
    template<typename T>
    static Single<T> Create(std::shared_ptr<ObservableCore> core) noexcept {
        return Single<T>{ std::move(core) };
    }

    template<typename T>
    static const std::shared_ptr<ObservableCore>& GetCore(const Single<T>& single) noexcept {
        return single.Core();
    }
};

}