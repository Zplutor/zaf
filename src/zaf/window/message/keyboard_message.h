#pragma once

#include <zaf/window/message/message.h>

namespace zaf {

/**
 Wraps information of a Win32 keyboard message.   
 */
class KeyboardMessage : public Message {
public:

};


/**
 Wraps information of a key message.
 */
class KeyMessage : public KeyboardMessage {
public:
    /**
     Get the virtual key.
     */
    DWORD GetVirtualKey() const {
        return wParam;
    }
};


/**
 Wraps information of a char message.
 */
class CharMessage : public KeyboardMessage {
public:
    /**
     Get the char.
     */
    wchar_t GetChar() const {
        return static_cast<wchar_t>(wParam);
    }
};


}