#include <zaf/rx/default_run_loop_thread.h>

namespace zaf::rx {

DefaultRunLoopThread::DefaultRunLoopThread() : 
    state_(std::make_shared<State>()),
    thread_(std::bind(&DefaultRunLoopThread::Run, state_)) {

}


DefaultRunLoopThread::~DefaultRunLoopThread() {

    state_->is_stopped.store(true);
    state_->work_event.notify_one();

    if (std::this_thread::get_id() == thread_.get_id()) {
        thread_.detach();
    }
    else {
        thread_.join();
    }
}


void DefaultRunLoopThread::PostWork(Closure work) {

    std::scoped_lock<std::mutex> lock(state_->works_lock);
    state_->works.push_back(std::move(work));
    state_->work_event.notify_one();
}


void DefaultRunLoopThread::Run(const std::shared_ptr<State>& state) {

    while (true) {

        std::unique_lock<std::mutex> lock(state->works_lock);

        if (state->works.empty()) {
            state->work_event.wait(lock, [&state]() {
                return state->is_stopped.load() || !state->works.empty();
            });
        }

        if (state->is_stopped.load()) {
            break;
        }

        auto work = std::move(state->works.front());
        state->works.pop_front();

        lock.unlock();
        work();
    }
}

}