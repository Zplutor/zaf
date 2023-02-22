#pragma once

#include <zaf/window/message/message_shim.h>

namespace zaf {

/**
 Wraps information of a Win32 keyboard message.   
 */
class KeyboardMessage : public MessageShim {
public:
    using MessageShim::MessageShim;
};


/**
 Wraps information of a key message.
 */
class KeyMessage : public KeyboardMessage {
public:
    using KeyboardMessage::KeyboardMessage;

    /**
     Get the virtual key.
     */
    DWORD VirtualKey() const {
        return static_cast<DWORD>(WParam());
    }
};


/**
 Wraps information of a char message.
 */
class CharMessage : public KeyboardMessage {
public:
    using KeyboardMessage::KeyboardMessage;

    /**
     Get the char.
     */
    wchar_t Char() const {
        return static_cast<wchar_t>(WParam());
    }
};


}