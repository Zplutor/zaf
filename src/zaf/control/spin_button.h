#pragma once

#include <zaf/control/arrow_direction.h>
#include <zaf/control/clickable_control.h>

namespace zaf {

class SpinButton : public zaf::ClickableControl {
public:
    ZAF_OBJECT;

    ArrowDirection Direction() const;
    void SetDirection(ArrowDirection direction);

    Color ArrowColor() const;
    void SetArrowColor(const Color& color);

    const ColorPicker& ArrowColorPicker() const;
    void SetArrowColorPicker(ColorPicker picker);

protected:
    void Initialize() override;
    void UpdateStyle() override;
    void Paint(Canvas& canvas, const zaf::Rect&) const override;

private:
    ArrowDirection direction_{ ArrowDirection::Up };

    internal::ColorField arrow_color_field_;
};

ZAF_OBJECT_BEGIN(SpinButton);
ZAF_OBJECT_PROPERTY(Direction);
ZAF_OBJECT_PROPERTY(ArrowColor);
ZAF_OBJECT_PROPERTY(ArrowColorPicker);
ZAF_OBJECT_END;

}