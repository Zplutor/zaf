#pragma once

#include <zaf/control/textual_control.h>

class MessageContentBubble : public zaf::TextualControl {
public:
    enum class Style {
        Normal,
        Highlight,
    };

public:
    void Initialize() override;

    void SetStyle(Style style) {
        style_ = style;
        NeedRepaint();
    }

protected:
    void Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) override;

private:
    Style style_ = Style::Normal;
};