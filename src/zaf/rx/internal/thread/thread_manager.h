#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/rx/default_run_loop_thread.h>
#include <zaf/rx/main_thread.h>
#include <zaf/rx/run_loop_thread.h>

namespace zaf::rx::internal {

class ThreadManager : NonCopyableNonMovable {
public:
    ThreadManager();
    ~ThreadManager();

    const std::shared_ptr<MainThread>& GetMainThread() const noexcept {
        return main_thread_;
    }

    std::shared_ptr<RunLoopThread> CreateNewThread();

private:
    std::shared_ptr<MainThread> main_thread_;
    std::vector<std::weak_ptr<DefaultRunLoopThread>> child_threads_;
    std::mutex child_threads_lock_;
};

}