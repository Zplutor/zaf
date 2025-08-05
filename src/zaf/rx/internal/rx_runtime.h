#pragma once

#include <zaf/rx/internal/thread/thread_manager.h>
#include <zaf/rx/internal/timer_manager.h>
#include <zaf/rx/scheduler/main_thread_scheduler.h>
#include <zaf/rx/scheduler/timer_scheduler.h>

namespace zaf::rx::internal {

class RxRuntime {
public:
    static RxRuntime& GetInstance();

public:
    RxRuntime();

    RxRuntime(const RxRuntime&) = delete;
    RxRuntime& operator=(const RxRuntime&) = delete;

    ThreadManager& GetThreadManager() {
        return thread_manager_;
    }

    TimerManager& GetTimerManager();

    const std::shared_ptr<MainThreadScheduler>& MainThreadScheduler() const noexcept {
        return main_thread_scheduler_;
    }

    const std::shared_ptr<TimerScheduler>& TimerScheduler() const noexcept {
        return timer_scheduler_;
    }

private:
    ThreadManager thread_manager_;
    std::unique_ptr<TimerManager> timer_manager_;
    std::once_flag timer_manager_once_flag_;

    std::shared_ptr<rx::MainThreadScheduler> main_thread_scheduler_;
    std::shared_ptr<rx::TimerScheduler> timer_scheduler_;
};

}