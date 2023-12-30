#pragma once

#include <zaf/window/message/message_shim.h>

namespace zaf {

/**
 Wraps information of a key message.
 */
class KeyMessage : public MessageShim {
public:
    using MessageShim::MessageShim;

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
class CharMessage : public MessageShim {
public:
    using MessageShim::MessageShim;

    /**
     Get the char.
     */
    wchar_t Char() const {
        return static_cast<wchar_t>(WParam());
    }
};


}