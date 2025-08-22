#pragma once

#include <zaf/rx/internal/insider/insider.h>
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


template<typename T>
class Insider<Single<T>> {
public:
    using Type = SingleInsider;
};

}