#pragma once

#include <zaf/window/message/internal/mouse_position_conversion.h>
#include <zaf/window/message/message_shim.h>

namespace zaf {

class HitTestMessage : public MessageShim {
public:
    using MessageShim::MessageShim;

    Point MousePosition() const {
        return internal::GetMousePositionFromLPARAM(WindowHandle(), LParam(), true);
    }
};

}