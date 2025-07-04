#pragma once

#include <zaf/rx/work.h>

namespace zaf::rx {

template<typename OBSERVABLE>
class TerminationCapability {
public:
    /**
    Returns an observable that invokes an action when the current observable is terminated, either
    by completion or error.

    @param action
        The action to be invoked upon termination of the current observable.

    @return
        A new observable.
    */
    OBSERVABLE DoOnTerminate(Work work) {
        const auto& core = static_cast<OBSERVABLE*>(this)->Core();
        return OBSERVABLE{ core->DoOnTerminate(std::move(work)) };
    }

    OBSERVABLE DoAfterTerminate(Work work) {
        const auto& core = static_cast<OBSERVABLE*>(this)->Core();
        return OBSERVABLE{ core->DoAfterTerminate(std::move(work)) };
    }
};

}