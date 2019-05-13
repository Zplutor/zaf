#pragma once

#include <zaf/window/message/internal/mouse_position_conversion.h>
#include <zaf/window/message/message.h>

namespace zaf {

class HitTestMessage : public Message {
public:
    Point GetMousePosition() const {
        return internal::GetMousePositionFromLPARAM(lparam, hwnd);
    }
};

}