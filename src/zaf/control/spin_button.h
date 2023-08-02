#pragma once

#include <zaf/control/arrow_direction.h>
#include <zaf/control/clickable_control.h>

namespace zaf {

class SpinButton : public zaf::ClickableControl {
public:
    ZAF_DECLARE_TYPE;

    ArrowDirection Direction() const;
    void SetDirection(ArrowDirection direction);

protected:
    void Paint(Canvas& canvas, const zaf::Rect&) override;

private:
    ArrowDirection direction_{ ArrowDirection::Up };
};

}