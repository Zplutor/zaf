#pragma once

#include <zaf/control/menu_item.h>

namespace zaf {

class MenuSeparator : public MenuItem {
public:
    ZAF_OBJECT;

protected:
    void Initialize() override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const override;
    zaf::Rect DetermineTextRect() override;
    zaf::Rect GetSubMenuArrowRect() const override;
    zaf::Size CalculatePreferredContentSize(const zaf::Size& bound_size) const override;
};

ZAF_OBJECT_BEGIN(MenuSeparator);
ZAF_OBJECT_END;

}