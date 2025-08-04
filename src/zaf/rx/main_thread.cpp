#include <zaf/rx/main_thread.h>
#include <zaf/base/error/win32_error.h>
#include <zaf/rx/internal/rx_runtime.h>

namespace zaf::rx {
namespace {

constexpr const wchar_t* const WindowClassName = L"ZAFMainThreadWindow";
constexpr DWORD DoWorkMessageId = WM_USER + 1;

}

void MainThread::RegisterWindowClass() {

    WNDCLASSEX class_info{};
    class_info.cbSize = sizeof(class_info);
    class_info.style = 0;
    class_info.lpfnWndProc = MainThread::WindowProcedure;
    class_info.cbClsExtra = 0;
    class_info.cbWndExtra = 0;
    class_info.hInstance = nullptr;
    class_info.hIcon = nullptr;
    class_info.hCursor = nullptr;
    class_info.hbrBackground = nullptr;
    class_info.lpszMenuName = nullptr;
    class_info.lpszClassName = WindowClassName;
    class_info.hIconSm = NULL;

    ATOM atom = RegisterClassEx(&class_info);
    if (!atom) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }
}


LRESULT CALLBACK MainThread::WindowProcedure(
    HWND hwnd,
    UINT message_id,
    WPARAM wparam,
    LPARAM lparam) {

    if (message_id == DoWorkMessageId) {

        auto work = reinterpret_cast<Closure*>(wparam);
        (*work)();
        delete work;
        return 0;
    }

    return CallWindowProc(DefWindowProc, hwnd, message_id, wparam, lparam);
}


const std::shared_ptr<MainThread>& MainThread::Instance() noexcept {
    return internal::RxRuntime::GetInstance().GetThreadManager().GetMainThread();
}


MainThread::MainThread() {

    RegisterWindowClass();

    window_handle_ = CreateWindow(
        WindowClassName,
        L"", 
        0, 
        0, 
        0, 
        0, 
        0, 
        HWND_MESSAGE, 
        nullptr, 
        nullptr, 
        0);

    if (!window_handle_) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }
}


MainThread::~MainThread() {
    DestroyWindow(window_handle_);
}


void MainThread::PostWork(Closure work) {

    auto cloned_work = std::make_unique<Closure>(std::move(work));
    BOOL is_succeeded = PostMessage(
        window_handle_, 
        DoWorkMessageId, 
        reinterpret_cast<WPARAM>(cloned_work.get()), 
        0);

    if (!is_succeeded) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }

    // Ownership is transferred to the message queue.
    cloned_work.release();
}


void MainThread::PostDelayedWork(std::chrono::steady_clock::duration delay, Closure work) {

    auto cloned_work = std::make_unique<Closure>(std::move(work));

    UINT_PTR result = SetTimer(
        window_handle_, 
        reinterpret_cast<UINT_PTR>(cloned_work.get()), 
        static_cast<UINT>(std::chrono::duration_cast<std::chrono::milliseconds>(delay).count()), 
        [](HWND hwnd, UINT, UINT_PTR id, DWORD) {
            std::unique_ptr<Closure> work{ reinterpret_cast<Closure*>(id) };
            (*work)();
            KillTimer(hwnd, id);
        });

    if (!result) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }

    // Ownership is transferred to the timer callback.
    cloned_work.release();  
}

}