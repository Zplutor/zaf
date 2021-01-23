#pragma once

#include <zaf/rx/internal/thread/thread.h>
#include <zaf/rx/internal/thread/run_loop_thread.h>
#include <zaf/rx/internal/thread/window_thread.h>

namespace zaf::internal {

class ThreadManager {
public:
    ThreadManager();

    ThreadManager(const ThreadManager&) = delete;
    ThreadManager& operator=(const ThreadManager&) = delete;

    std::shared_ptr<Thread> GetMainThread() const {
        return main_thread_;
    }

    std::shared_ptr<Thread> CreateNewThread();

private:
    std::shared_ptr<WindowThread> main_thread_;

    std::vector<std::weak_ptr<RunLoopThread>> child_threads_;
    std::mutex child_threads_lock_;
};

}