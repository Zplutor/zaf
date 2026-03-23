#pragma once

#include <zaf/window/message/message_shim.h>

namespace zaf {

class IMECompositionMessage : public MessageShim {
public:
    using MessageShim::MessageShim;

    bool HasResultString() const noexcept;
    bool HasCompositionString() const noexcept;
    bool HasCaretPosition() const noexcept;
};


class IMERequestMessage : public MessageShim {
public:
    using MessageShim::MessageShim;
};

}