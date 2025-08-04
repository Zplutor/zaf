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

    auto cloned_work = new Closure(std::move(work));
    PostMessage(window_handle_, DoWorkMessageId, reinterpret_cast<WPARAM>(cloned_work), 0);
}

}