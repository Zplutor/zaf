#include <zaf/rx/thread/main_thread.h>
#include <zaf/base/error/contract_error.h>
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
    class_info.cbWndExtra = sizeof(LONG_PTR);
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

    if (message_id == WM_NCCREATE) {
        auto create_struct = reinterpret_cast<const CREATESTRUCTA*>(lparam);
        SetWindowLongPtr(
            hwnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(create_struct->lpCreateParams));
    }
    else if (message_id == DoWorkMessageId) {
        auto work = reinterpret_cast<Closure*>(wparam);
        (*work)();
        delete work;
        return 0;
    }
    else if (message_id == WM_TIMER) {
        LONG_PTR user_data = GetWindowLongPtr(hwnd, GWLP_USERDATA);
        auto main_thread = reinterpret_cast<MainThread*>(user_data);
        if (main_thread) {
            main_thread->OnTimer(static_cast<UINT_PTR>(wparam));
        }
        return 0;
    }

    return CallWindowProc(DefWindowProc, hwnd, message_id, wparam, lparam);
}


const std::shared_ptr<MainThread>& MainThread::Instance() noexcept {
    return internal::RxRuntime::GetInstance().GetThreadManager().GetMainThread();
}


MainThread::MainThread() : state_(std::make_shared<State>()) {

    RegisterWindowClass();

    state_->window_handle = CreateWindow(
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
        this);

    if (!state_->window_handle) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }
}


MainThread::~MainThread() {
    DestroyWindow(state_->window_handle);
}


void MainThread::PostWork(Closure work) {

    ZAF_EXPECT(work);

    auto cloned_work = std::make_unique<Closure>(std::move(work));
    BOOL is_succeeded = PostMessage(
        state_->window_handle,
        DoWorkMessageId, 
        reinterpret_cast<WPARAM>(cloned_work.get()), 
        0);

    if (!is_succeeded) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }

    // Ownership is transferred to the message queue.
    cloned_work.release();
}


std::shared_ptr<Disposable> MainThread::PostDelayedWork(
    std::chrono::steady_clock::duration delay, 
    Closure work) {

    ZAF_EXPECT(work);

    auto work_item = std::make_shared<DelayedWorkItem>(std::move(work), state_);
    state_->AddDelayedWorkItem(work_item);

    UINT_PTR result = SetTimer(
        state_->window_handle,
        reinterpret_cast<UINT_PTR>(work_item.get()),
        static_cast<UINT>(std::chrono::duration_cast<std::chrono::milliseconds>(delay).count()),
        nullptr);

    if (!result) {

        auto last_error = GetLastError();
        // Remove the work item from the collection if the timer creation fails.
        state_->TakeDelayedWorkItem(work_item.get());
        ZAF_THROW_WIN32_ERROR(last_error);
    }

    return work_item;
}


void MainThread::OnTimer(UINT_PTR timer_id) {

    // The timer of delayed work item is one-shot, so it will be killed after being triggered.
    KillTimer(state_->window_handle, timer_id);

    auto work_item_pointer = reinterpret_cast<DelayedWorkItem*>(timer_id);
    auto work_item = state_->TakeDelayedWorkItem(work_item_pointer);
    if (work_item) {

        auto work = work_item->TakeWorkIfNotDisposed();
        if (work) {
            work();
        }
    }
}


void MainThread::State::AddDelayedWorkItem(std::shared_ptr<DelayedWorkItem> work_item) {

    std::lock_guard<std::mutex> lock(lock_);

    auto insert_iterator = std::lower_bound(
        delayed_work_items_.begin(),
        delayed_work_items_.end(),
        work_item);

    delayed_work_items_.insert(insert_iterator, std::move(work_item));
}


std::shared_ptr<MainThread::DelayedWorkItem> MainThread::State::TakeDelayedWorkItem(
    DelayedWorkItem* item_pointer) noexcept {

    std::lock_guard<std::mutex> lock(lock_);

    auto iterator = std::lower_bound(
        delayed_work_items_.begin(),
        delayed_work_items_.end(),
        item_pointer,
        [](const std::shared_ptr<DelayedWorkItem>& item, DelayedWorkItem* pointer) {
            return item.get() < pointer;
        });

    if (iterator == delayed_work_items_.end() || iterator->get() != item_pointer) {
        return nullptr;
    }

    auto result = std::move(*iterator);
    delayed_work_items_.erase(iterator);
    return result;
}


MainThread::DelayedWorkItem::DelayedWorkItem(Closure work, std::weak_ptr<State> state) : 
    DelayedWorkItemBase(std::move(work)),
    state_(std::move(state)) {

}


void MainThread::DelayedWorkItem::OnDispose() noexcept {

    auto state = state_.lock();
    if (!state) {
        return;
    }

    KillTimer(state->window_handle, reinterpret_cast<UINT_PTR>(this));
    state->TakeDelayedWorkItem(this);
}

}