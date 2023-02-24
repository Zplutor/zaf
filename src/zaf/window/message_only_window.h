#pragma once

#include <zaf/window/internal/inner_message_only_window.h>

namespace zaf {

/**
A window that is used only for receiving messages.
*/
class MessageOnlyWindow {
public:
    MessageOnlyWindow() : inner_(HWND_MESSAGE) { }

    HWND Handle() const {
        return inner_.Handle();
    }

    Observable<Message> MessageReceivedEvent() {
        return inner_.MessageReceivedEvent();
    }

private:
    internal::InnerMessageOnlyWindow inner_;
};

}