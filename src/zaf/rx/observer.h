#pragma once

#include <memory>
#include <zaf/rx/internal/observer_core.h>

namespace zaf::rx::internal {
template<typename T>
class ObservableFactory;
}

namespace zaf::rx {

template<typename T>
class Observer {
public:
    static Observer Create() {
        return Create(nullptr, nullptr, nullptr);
    }

    static Observer Create(OnNext<T> on_next) {
        return Create(std::move(on_next), nullptr, nullptr);
    }

    static Observer Create(OnNext<T> on_next, OnError on_error) {
        return Create(std::move(on_next), std::move(on_error), nullptr);
    }

    static Observer Create(OnNext<T> on_next, OnCompleted on_completed) {
        return Create(std::move(on_next), nullptr, std::move(on_completed));
    }

    static Observer Create(OnNext<T> on_next, OnError on_error, OnCompleted on_completed) {

        auto bridged_on_next = [on_next](const std::any& value) {
            if (on_next) {
                on_next(std::any_cast<T>(value));
            }
        };

        return Observer(rx::internal::ObserverCore::Create(
            std::move(bridged_on_next), 
            std::move(on_error), 
            std::move(on_completed)));
    }

    static Observer Create(OnError on_error) {
        return Observer(rx::internal::ObserverCore::Create(nullptr, std::move(on_error), nullptr));
    }

public:
    void OnNext(const T& value) const {
        core_->OnNext(value);
    }

    void OnError(const std::exception_ptr& error) const {
        core_->OnError(error);
    }

    template<typename E>
    void OnError(E error) const {
        this->OnError(std::make_exception_ptr(std::move(error)));
    }

    void OnCompleted() const {
        core_->OnCompleted();
    }

    const std::shared_ptr<rx::internal::ObserverCore>& Core() const noexcept {
        return core_;
    }

protected:
    explicit Observer(std::shared_ptr<rx::internal::ObserverCore> core) noexcept :
        core_(std::move(core)) {
    }

private:
    friend class rx::internal::ObservableFactory<T>;

private:
    std::shared_ptr<rx::internal::ObserverCore> core_;
};

}