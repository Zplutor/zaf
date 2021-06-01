#pragma once

#include <zaf/window/internal/inner_message_only_window.h>

namespace zaf {

class MessageOnlyWindow {
public:
    MessageOnlyWindow() : inner_(HWND_MESSAGE) { }

    HWND GetHandle() const {
        return inner_.GetHandle();
    }

    Observable<Message> ReceiveMessageEvent() {
        return inner_.ReceiveMessageEvent();
    }

private:
    internal::InnerMessageOnlyWindow inner_;
};

}