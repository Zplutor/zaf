#pragma once

#include <zaf/window/message/message_shim.h>

namespace zaf {

/**
A message shim to access parameters of WM_SETFOCUS message.
*/
class SetFocusMessage : public MessageShim {
public:
    using MessageShim::MessageShim;

    HWND LostFocusWindowHandle() const {
        return reinterpret_cast<HWND>(WParam());
    }
};


/**
A message shim to access parameters of WM_KILLFOCUS message.
*/
class KillFocusMessage : public MessageShim {
public:
    using MessageShim::MessageShim;

    HWND GainedFocusWindowHandle() const {
        return reinterpret_cast<HWND>(WParam());
    }
};

}