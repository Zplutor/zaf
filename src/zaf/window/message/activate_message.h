#pragma once

#include <zaf/window/message/message_shim.h>

namespace zaf {

enum class ActivateState {

    Active = WA_ACTIVE,
    ClickActive = WA_CLICKACTIVE,
    Inactive = WA_INACTIVE,
};


class ActivateMessage : public MessageShim {
public:
    using MessageShim::MessageShim;

    zaf::ActivateState ActivateState() const {
        return static_cast<zaf::ActivateState>(LOWORD(Inner().wparam));
    }

    bool IsWindowMinimized() const {
        return (HIWORD(Inner().wparam) != 0);
    }

    HWND EffectingWindowHandle() const {
        return reinterpret_cast<HWND>(Inner().lparam);
    }
};

}