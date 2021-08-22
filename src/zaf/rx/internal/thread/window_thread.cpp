#include <zaf/rx/internal/thread/window_thread.h>
#include <zaf/base/error/system_error.h>

namespace zaf::internal {
namespace {

constexpr const wchar_t* const WindowClassName = L"ZafWindowedThreadWindowClass";
constexpr DWORD DoWorkMessageId = WM_USER + 1;

}

void WindowThread::RegisterWindowClass() {

    WNDCLASSEX class_info{};
    class_info.cbSize = sizeof(class_info);
    class_info.style = 0;
    class_info.lpfnWndProc = WindowThread::WindowProcedure;
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
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }
}


LRESULT CALLBACK WindowThread::WindowProcedure(
    HWND hwnd,
    UINT message_id,
    WPARAM wparam,
    LPARAM lparam) {

    if (message_id == DoWorkMessageId) {

        auto work = reinterpret_cast<Work*>(wparam);
        (*work)();
        delete work;
        return 0;
    }

    return CallWindowProc(DefWindowProc, hwnd, message_id, wparam, lparam);
}


WindowThread::WindowThread() {

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
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }
}


WindowThread::~WindowThread() {

    DestroyWindow(window_handle_);
}


void WindowThread::DoWork(Work work) {

    auto cloned_word = new Work(std::move(work));
    PostMessage(window_handle_, DoWorkMessageId, reinterpret_cast<WPARAM>(cloned_word), 0);
}

}