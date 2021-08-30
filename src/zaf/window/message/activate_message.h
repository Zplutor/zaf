#pragma once

#include <zaf/window/message/message.h>

namespace zaf {

enum class ActivateState {

    Active = WA_ACTIVE,
    ClickActive = WA_CLICKACTIVE,
    Inactive = WA_INACTIVE,
};

class ActivateMessage : public Message {
public:
    ActivateState State() const {
        return static_cast<ActivateState>(LOWORD(wparam));
    }

    bool IsWindowMinimized() const {
        return (HIWORD(wparam) != 0);
    }

    HWND WindowHandle() const {
        return reinterpret_cast<HWND>(lparam);
    }
};

}