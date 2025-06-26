#include <zaf/rx/internal/thread/thread_manager.h>
#include <zaf/base/container/utility/erase.h>

namespace zaf::rx::internal {

ThreadManager::ThreadManager() : main_thread_(std::make_shared<WindowThread>()) {

}


std::shared_ptr<Thread> ThreadManager::CreateNewThread() {

    auto new_thread = std::make_shared<RunLoopThread>();

    std::scoped_lock<std::mutex> lock(child_threads_lock_);

    EraseIf(child_threads_, [](const auto& each_thread) {
        return !each_thread.lock();
    });

    child_threads_.push_back(new_thread);
    return new_thread;
}


}