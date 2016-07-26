#pragma once

#include <Windows.h>

namespace zaf {

/**
 Wraps information of a Win32 message.   
 */
class Message {
public:
    /**
     Construct the instance.
     */
    Message() : hwnd(nullptr), id(0), wParam(0), lParam(0) { }

    /**
     Destruct the instance.
     */
    virtual ~Message() { }

public:
    /**
     The window handle.
     */
    HWND hwnd;

    /**
     The message identifier.
     */
    UINT id;

    /**
     The WPARAM argument.
     */
    WPARAM wParam;

    /**
     The LPARAM argument.
     */
    LPARAM lParam;
};

}