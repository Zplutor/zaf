#pragma once

#include <zaf/window/internal/inner_message_only_window.h>

namespace zaf::internal {

class SystemMessageWindow {
public:
    SystemMessageWindow();

    rx::Observable<Message> MessageReceivedEvent() {
        return message_only_window_.MessageReceivedEvent();
    }

private:
    InnerMessageOnlyWindow message_only_window_;
};

}