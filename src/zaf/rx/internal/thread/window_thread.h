#pragma once

#include <Windows.h>
#include <zaf/rx/internal/thread/thread.h>

namespace zaf::internal {

class WindowThread : public Thread {
public:
    WindowThread();
    ~WindowThread();

    void DoWork(Work work) override;

private:
    static void RegisterWindowClass();

    static LRESULT CALLBACK WindowProcedure(
        HWND hwnd, 
        UINT message_id, 
        WPARAM wparam, 
        LPARAM lparam);

private:
    HWND window_handle_{};
};

}