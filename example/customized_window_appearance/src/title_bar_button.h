#pragma once

#include <zaf/control/clickable_control.h>

class TitleBarButton : public zaf::ClickableControl {
public:
    enum class Type {
        Close,
        Minimize,
        Maximize,
    };

public:
    TitleBarButton(Type type);

    void Initialize() override;
    void Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) override;

private:
    zaf::Color GetBackgroundColor() const;
    zaf::Color GetBorderColor() const;

private:
    Type type_{};
};