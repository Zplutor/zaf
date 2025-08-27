#include <zaf/rx/scheduler/new_thread_scheduler.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/rx/thread/default_run_loop_thread.h>

namespace zaf::rx {

const std::shared_ptr<NewThreadScheduler>& NewThreadScheduler::Default() {
    static auto instance = std::make_shared<NewThreadScheduler>();
    return instance;
}


NewThreadScheduler::NewThreadScheduler() : shared_state_(std::make_shared<SharedState>()) {

}


NewThreadScheduler::~NewThreadScheduler() {

}


std::shared_ptr<Disposable> NewThreadScheduler::ScheduleWork(Closure work) {
    return ScheduleWorkOnThread(std::move(work), std::nullopt);
}


std::shared_ptr<Disposable> NewThreadScheduler::ScheduleDelayedWork(
    std::chrono::steady_clock::duration delay,
    Closure work) {

    return ScheduleWorkOnThread(std::move(work), delay);
}


std::shared_ptr<Disposable> NewThreadScheduler::ScheduleWorkOnThread(
    Closure work, 
    std::optional<std::chrono::steady_clock::duration> delay) {

    ZAF_EXPECT(work);

    auto work_item = std::make_shared<WorkItem>(std::move(work), shared_state_);
    {
        std::lock_guard<std::mutex> lock(shared_state_->mutex);
        shared_state_->work_items.insert(work_item);
    }

    try {
        work_item->RunOnThread(delay);
        return work_item;
    }
    catch (...) {
        {
            std::lock_guard<std::mutex> lock(shared_state_->mutex);
            shared_state_->work_items.erase(work_item);
        }
        throw;
    }
}


std::size_t NewThreadScheduler::ThreadCount() const noexcept {
    std::lock_guard<std::mutex> lock(shared_state_->mutex);
    return shared_state_->work_items.size();
}


NewThreadScheduler::WorkItem::WorkItem(
    Closure work, 
    std::weak_ptr<SharedState> shared_state) 
    :
    ThreadWorkItemBase(std::move(work)),
    shared_state_(std::move(shared_state)),
    thread_(std::make_unique<DefaultRunLoopThread>()) {

}


void NewThreadScheduler::WorkItem::RunOnThread(
    std::optional<std::chrono::steady_clock::duration> delay) {

    auto thread_work = [weak_this = weak_from_this()]() {
        auto shared_this = weak_this.lock();
        if (shared_this) {
            shared_this->RunWork();
        }
    };

    if (delay) {
        work_disposable_ = thread_->PostDelayedWork(*delay, std::move(thread_work));
    }
    else {
        work_disposable_ = thread_->PostWork(std::move(thread_work));
    }
}


void NewThreadScheduler::WorkItem::OnDispose() noexcept {

    if (work_disposable_) {
        work_disposable_->Dispose();
        work_disposable_.reset();
    }

    auto shared_state = shared_state_.lock();
    if (shared_state) {
        std::lock_guard<std::mutex> lock(shared_state->mutex);
        shared_state->work_items.erase(shared_from_this());
    }

    // Stop the thread.
    thread_.reset();
}


NewThreadScheduler::WorkItem::~WorkItem() {
    if (work_disposable_) {
        work_disposable_->Dispose();
    }
}

}