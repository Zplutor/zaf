#pragma once

#include <Windows.h>
#include <mutex>
#include <zaf/rx/internal/thread/delayed_work_item_base.h>
#include <zaf/rx/thread/run_loop_thread.h>

namespace zaf::rx::internal {
class ThreadManager;
}

namespace zaf::testing {
class MainThreadTest;
}

namespace zaf::rx {

/**
Represents the main thread of the application.
*/
class MainThread : public RunLoopThread {
public:
    static const std::shared_ptr<MainThread>& Instance() noexcept;

public:
    ~MainThread();

    void PostWork(Closure work) override;

    std::shared_ptr<Disposable> PostDelayedWork(
        std::chrono::steady_clock::duration delay, 
        Closure work) override;

private:
    class DelayedWorkItem;

    class State {
    public:
        void AddDelayedWorkItem(std::shared_ptr<DelayedWorkItem> work_item);

        std::shared_ptr<DelayedWorkItem> TakeDelayedWorkItem(
            DelayedWorkItem* item_pointer) noexcept;

    public:
        HWND window_handle{};

    private:
        std::mutex lock_;
        // Delayed work items, sorted by their pointer addresses.
        std::vector<std::shared_ptr<DelayedWorkItem>> delayed_work_items_;
    };

    class DelayedWorkItem : public internal::DelayedWorkItemBase {
    public:
        DelayedWorkItem(Closure work, std::weak_ptr<State> state);

    protected:
        void OnDispose() noexcept override;

    private:
        std::weak_ptr<State> state_;
    };

private:
    static void RegisterWindowClass();

    static LRESULT CALLBACK WindowProcedure(
        HWND hwnd, 
        UINT message_id, 
        WPARAM wparam, 
        LPARAM lparam);

private:
    friend class internal::ThreadManager;
    friend class testing::MainThreadTest;

    MainThread();

    HWND WindowHandle() const noexcept {
        return state_->window_handle;
    }

    void OnTimer(UINT_PTR timer_id);

private:
    std::shared_ptr<State> state_;
};

}