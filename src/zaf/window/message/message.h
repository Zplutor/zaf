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
        hwnd(hwnd),
        id(id), 
        wparam(wparam),
        lparam(lparam) { }

public:
    /**
     The window handle.
     */
    HWND hwnd{};

    /**
     The message identifier.
     */
    UINT id{};

    /**
     The WPARAM argument.
     */
    WPARAM wparam{};

    /**
     The LPARAM argument.
     */
    LPARAM lparam{};
};

}