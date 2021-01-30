#pragma once

#include <zaf/rx/internal/thread/thread_manager.h>
#include <zaf/rx/internal/timer_manager.h>

namespace zaf::internal {

class RxRuntime {
public:
    static RxRuntime& GetInstance();

public:
    RxRuntime() = default;

    RxRuntime(const RxRuntime&) = delete;
    RxRuntime& operator=(const RxRuntime&) = delete;

    ThreadManager& GetThreadManager() {
        return thread_manager_;
    }

    TimerManager& GetTimerManager();

private:
    ThreadManager thread_manager_;
    std::unique_ptr<TimerManager> timer_manager_;
    std::once_flag timer_manager_once_flag_;
};

}