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
    Message() : hwnd(nullptr), id(0), wparam(0), lparam(0) { }

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
    WPARAM wparam;

    /**
     The LPARAM argument.
     */
    LPARAM lparam;
};

}