#pragma once

#include <zaf/rx/internal/observable/just_observable.h>
#include <zaf/rx/work.h>

namespace zaf::rx {

/**
A mixin base class for observables that have termination capabilities.
*/
template<
    template<typename> typename OBSERVABLE,
    typename T
>
class TerminationCapability {
public:
    static OBSERVABLE<T> Just(T value) {
        auto core = std::make_shared<internal::JustObservable>(std::any{ std::move(value) });
        return OBSERVABLE<T>{ std::move(core) };
    }

public:
    /**
    Returns an observable that invokes an action when the current observable is terminated, either
    by completion or error.

    @param action
        The action to be invoked upon termination of the current observable.

    @return
        A new observable.
    */
    OBSERVABLE<T> DoOnTerminate(Work work) {
        const auto& core = static_cast<OBSERVABLE<T>*>(this)->Core();
        return OBSERVABLE<T>{ core->DoOnTerminate(std::move(work)) };
    }

    OBSERVABLE<T> DoAfterTerminate(Work work) {
        const auto& core = static_cast<OBSERVABLE<T>*>(this)->Core();
        return OBSERVABLE<T>{ core->DoAfterTerminate(std::move(work)) };
    }
};

}