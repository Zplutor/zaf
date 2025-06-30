#pragma once

#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/observer_functions.h>

namespace zaf::rx {

template<typename T>
class ContinuousObserver {
public:
    static ContinuousObserver Create(OnNext<T> on_next) {

        auto on_next_bridge = [on_next = std::move(on_next)](const std::any& value) {
            if (on_next) {
                on_next(std::any_cast<T>(value));
            }
        };

        return ContinuousObserver{
            internal::ObserverCore::Create(
                std::move(on_next_bridge),
                nullptr,
                nullptr)
        };
    }

public:
    explicit ContinuousObserver(std::shared_ptr<internal::ObserverCore> core) noexcept :
        core_(std::move(core)) {

    }

    void OnNext(const T& value) {
        core_->OnNext(value);
    }

    const std::shared_ptr<internal::ObserverCore>& Core() const noexcept {
        return core_;
    }

private:
    std::shared_ptr<internal::ObserverCore> core_;
};

}