#pragma once

#include <zaf/rx/observable.h>
#include <zaf/rx/subject.h>
#include <zaf/window/message/message.h>

namespace zaf {

class MessageOnlyWindow {
public:
    MessageOnlyWindow();
    ~MessageOnlyWindow();

    MessageOnlyWindow(const MessageOnlyWindow&) = delete;
    MessageOnlyWindow& operator=(const MessageOnlyWindow&) = delete;

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