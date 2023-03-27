#pragma once

#include <zaf/control/menu_item.h>

namespace zaf {

class MenuSeparator : public MenuItem {
protected:
    void Initialize() override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) override;
    zaf::Rect GetTextRect() override;
    zaf::Rect GetSubMenuArrowRect() override;
    zaf::Size CalculatePreferredContentSize(const zaf::Size& bound_size) const override;
};

}