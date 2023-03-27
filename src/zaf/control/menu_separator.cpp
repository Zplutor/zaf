#include <zaf/control/menu_separator.h>
#include <zaf/graphic/canvas.h>

namespace zaf {

void MenuSeparator::Initialize() {

    __super::Initialize();

    SetIsEnabled(false);
    SetFixedHeight(9);

    SetPadding(Frame{ 4, 0, 4, 0 });
}


void MenuSeparator::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    auto content_rect = ContentRect();

    float line_y = content_rect.Top() + std::floor(content_rect.size.height / 2);
    
    Point line_begin_point{ content_rect.Left(), line_y };
    Point line_end_point{ content_rect.Right(), line_y };

    Canvas::StateGuard state_guard(canvas);
    canvas.SetBrushWithColor(Color::Gray());
    canvas.DrawLine(line_begin_point, line_end_point, 1);
}


zaf::Rect MenuSeparator::GetTextRect() {
    //Don't draw any text for separator.
    return {};
}


zaf::Rect MenuSeparator::GetSubMenuArrowRect() {
    //Separator doesn't have any sub menu item.
    return {};
}


zaf::Size MenuSeparator::CalculatePreferredContentSize(const zaf::Size& bound_size) const {
    //Separator doesn't have preferred content size.
    //Its height is fixed, and its width is determinated by other menu items.
    return {};
}

}