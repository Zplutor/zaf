#pragma once

#include <zaf/input/key.h>
#include <zaf/window/message/message_shim.h>

namespace zaf {

/**
 Wraps information of a key message.
 */
class KeyMessage : public MessageShim {
public:
    using MessageShim::MessageShim;

    /**
    Gets the key.
    */
    zaf::Key Key() const {
        return static_cast<zaf::Key>(WParam());
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