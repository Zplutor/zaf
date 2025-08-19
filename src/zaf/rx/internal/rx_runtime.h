#pragma once

#include <zaf/rx/internal/thread/thread_manager.h>
#include <zaf/rx/schedulers/main_thread_scheduler.h>

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

    const std::shared_ptr<MainThreadScheduler>& MainThreadScheduler() const noexcept {
        return main_thread_scheduler_;
    }

private:
    ThreadManager thread_manager_;
    std::shared_ptr<rx::MainThreadScheduler> main_thread_scheduler_;
};

}