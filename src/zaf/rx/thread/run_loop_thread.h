#pragma once

/**
@file
    Defines the `zaf::rx::RunLoopThread` class.
*/

#include <chrono>
#include <zaf/base/closure.h>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/disposable.h>

namespace zaf::rx {

/**
Represents a thread that runs a run loop.
*/
class RunLoopThread : NonCopyableNonMovable {
public:
    RunLoopThread() = default;
    virtual ~RunLoopThread() = default;

    /**
    Posts a work to the thread which will be executed without delay.

    @param work
        The work to be executed.

    @pre
        The work is not null.

    @return
        A disposable object that can be used to cancel the work if it is not being executed or is 
        not ready to be executed. It is guaranteed that a work posted and then cancelled on the 
        thread itself during the same run loop iteration will not be executed.

    @post
        The returned object is not null.

    @throw zaf::PreconditionError
    @throw ...
        Any exception thrown by the run loop thread implementations.

    @details
        If the work is posted on the thread itself, it will be executed on the next run loop 
        iteration.
    */
    virtual std::shared_ptr<Disposable> PostWork(Closure work) = 0;

    /**
    Posts a work to the thread which will be executed after a delay.

    @param delay
        The delay before executing the work.

    @param work
        The work to be executed.

    @pre
        The work is not null.

    @return
        A disposable object that can be used to cancel the delayed work.

    @post
        The returned object is not null.

    @throw zaf::PreconditionError
    @throw ...
        Any exception thrown by the run loop thread implementations.
    */
    virtual std::shared_ptr<Disposable> PostDelayedWork(
        std::chrono::steady_clock::duration delay, 
        Closure work) = 0;
};

}