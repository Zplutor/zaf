#pragma once

#include <Windows.h>
#include <zaf/rx/run_loop_thread.h>

namespace zaf::rx::internal {
class ThreadManager;
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

private:
    static void RegisterWindowClass();

    static LRESULT CALLBACK WindowProcedure(
        HWND hwnd, 
        UINT message_id, 
        WPARAM wparam, 
        LPARAM lparam);

private:
    friend class internal::ThreadManager;

    MainThread();

private:
    HWND window_handle_{};
};

}