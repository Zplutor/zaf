#pragma once

#include <Windows.h>

namespace zaf {

/**
 Wraps information of a Win32 message.   
 */
class Message {
public:
    Message() = default;

    Message(HWND hwnd, UINT id, WPARAM wparam, LPARAM lparam) : 
        hwnd_(hwnd),
        id_(id), 
        wparam_(wparam),
        lparam_(lparam) { }

public:
    /**
     The window handle.
     */
    HWND WindowHandle() const {
        return hwnd_;
    }

    /**
     The message identifier.
     */
    UINT ID() const {
        return id_;
    }

    /**
     The WPARAM argument.
     */
    WPARAM WParam() const {
        return wparam_;
    }

    /**
     The LPARAM argument.
     */
    LPARAM LParam() const {
        return lparam_;
    }

private:
    HWND hwnd_{};
    UINT id_{};
    WPARAM wparam_{};
    LPARAM lparam_{};
};

}