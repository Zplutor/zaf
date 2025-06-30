#pragma once

#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::rx {

template<template<typename> typename OBSERVABLE, typename T>
class BaseObservable {
public:
    /**
    Returns an observable that invokes an action when the current observable is terminated, either
    by completion or error.

    @param action
        The action to be invoked upon termination of the current observable.

    @return
        A new observable.
    */
    OBSERVABLE<T> DoOnTerminated(Work work) {
        return OBSERVABLE<T>{ core_->DoOnTerminated(std::move(work)) };
    }

    OBSERVABLE<T> Finally(Work work) {
        return OBSERVABLE<T>{ core_->Finally(std::move(work)) };
    }

    template<typename K>
    OBSERVABLE<K> Map(std::function<K(const T&)> mapper) {
        auto new_core = core_->Map([mapper = std::move(mapper)](const std::any& value) {
            return mapper(std::any_cast<T>(value));
        });
        return OBSERVABLE<K>{ std::move(new_core) };
    }

    template<typename K>
    OBSERVABLE<K> FlatMap(std::function<OBSERVABLE<K>(const T&)> mapper) {
        auto new_core = core_->FlatMap([mapper = std::move(mapper)](const std::any& value) {
            return mapper(std::any_cast<T>(value)).Core();
        });
        return OBSERVABLE<K>{ std::move(new_core) };
    }

    const std::shared_ptr<internal::ObservableCore>& Core() const noexcept {
        return core_;
    }

protected:
    explicit BaseObservable(std::shared_ptr<internal::ObservableCore> core) noexcept :
        core_(std::move(core)) {

    }

private:
    std::shared_ptr<internal::ObservableCore> core_;
};

}
