#pragma once

#include <zaf/rx/internal/observer_core.h>

namespace zaf::rx::internal {
template<typename T>
class OnceFactory;
}

namespace zaf::rx {

template<typename T>
using OnDone = OnNext<T>;

template<typename T>
class OnceObserver {
public:
    static OnceObserver<T> Create() {
        return OnceObserver<T>{
            internal::ObserverCore::Create(
                nullptr,
                nullptr,
                nullptr)
        };
    }

    static OnceObserver<T> Create(OnDone<T> on_done) {

        auto on_done_bridge = [on_done = std::move(on_done)](const std::any& value) {
            if (on_done) {
                on_done(std::any_cast<T>(value));
            }
        };

        return OnceObserver<T>{
            internal::ObserverCore::Create(
                std::move(on_done_bridge),
                nullptr,
                nullptr)
        };
    }

public:
    void OnDone(const T& value) {
        core_->OnNext(value);
        core_->OnCompleted();
    }

    const std::shared_ptr<internal::ObserverCore>& Core() const noexcept {
        return core_;
    }

private:
    friend class internal::OnceFactory<T>;

    explicit OnceObserver(std::shared_ptr<internal::ObserverCore> core) :
        core_(std::move(core)) {
    }

private:
    std::shared_ptr<internal::ObserverCore> core_;
};

}