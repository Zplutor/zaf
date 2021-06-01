#pragma once

#include <zaf/rx/observable.h>
#include <zaf/rx/subject.h>
#include <zaf/window/message/message.h>

namespace zaf::internal {

class InnerMessageOnlyWindow {
public:
    InnerMessageOnlyWindow(HWND parent_window_handle);
    ~InnerMessageOnlyWindow();

    InnerMessageOnlyWindow(const InnerMessageOnlyWindow&) = delete;
    InnerMessageOnlyWindow& operator=(const InnerMessageOnlyWindow&) = delete;

    HWND GetHandle() const {
        return handle_;
    }

    Observable<Message> ReceiveMessageEvent() {
        return subject_.GetObservable();
    }

private:
    static void RegisterWindowClass();
    static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT id, WPARAM wparam, LPARAM lparam);

private:
    void OnReceiveMessage(UINT id, WPARAM wparam, LPARAM lparam);

private:
    HWND handle_{};
    Subject<Message> subject_;
};

}