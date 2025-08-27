#pragma once

/**
@file
    Defines the `zaf::rx::MainThreadScheduler` class.
*/

#include <zaf/rx/scheduler/single_thread_scheduler.h>
#include <zaf/rx/thread/main_thread.h>

namespace zaf::rx::internal {
class RxRuntime;
}

namespace zaf::rx {

/**
Represents a scheduler that executes works in the main thread.
*/
class MainThreadScheduler : public SingleThreadScheduler {
public:
    /**
    Gets the singleton instance of `zaf::rx::MainThreadScheduler`.

    @return
        The singleton instance.

    @post
        The return object is not null.

    @details
        @note
            This method is valid only if `zaf::rx::Application::Initialize()` has been called.
    */
    static const std::shared_ptr<MainThreadScheduler>& Instance() noexcept;

private:
    friend class internal::RxRuntime;

    explicit MainThreadScheduler(std::shared_ptr<MainThread> main_thread);
};

}