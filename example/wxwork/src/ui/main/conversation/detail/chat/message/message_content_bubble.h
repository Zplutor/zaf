#pragma once

#include <zaf/control/textual_control.h>

class MessageContentBubble : public zaf::TextualControl {
public:
    void Initialize() override;

protected:
    void Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) override;
};