#pragma once

#include <deque>
#include <mutex>
#include <thread>
#include <zaf/rx/internal/thread/thread.h>

namespace zaf::rx::internal {

class RunLoopThread : public Thread {
public:
    RunLoopThread();
    ~RunLoopThread();

    void DoWork(Work work) override;

private:
    class State {
    public:
        std::atomic<bool> is_stopped{};
        std::deque<Work> works;
        std::mutex works_lock;
        std::condition_variable work_event;
    };

private:
    static void Run(const std::shared_ptr<State>& state);

private:
    std::shared_ptr<State> state_;
    std::thread thread_;
};

}