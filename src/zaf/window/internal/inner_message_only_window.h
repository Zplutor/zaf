#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/subjects/subject.h>
#include <zaf/window/message/message.h>

namespace zaf::internal {

class InnerMessageOnlyWindow : public NonCopyableNonMovable {
public:
    InnerMessageOnlyWindow(HWND parent_window_handle);
    ~InnerMessageOnlyWindow();

    HWND Handle() const {
        return handle_;
    }

    rx::Observable<Message> MessageReceivedEvent() {
        return subject_.AsObservable();
    }

private:
    static void RegisterWindowClass();
    static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT id, WPARAM wparam, LPARAM lparam);

private:
    void OnMessageReceived(UINT id, WPARAM wparam, LPARAM lparam);

private:
    HWND handle_{};
    rx::Subject<Message> subject_;
};

}