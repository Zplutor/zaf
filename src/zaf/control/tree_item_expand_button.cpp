#include <zaf/control/tree_item_expand_button.h>
#include <zaf/control/internal/triangle_geometry.h>
#include <zaf/graphic/canvas.h>

namespace zaf {

void TreeItemExpandButton::Initialize() {

    __super::Initialize();

    SetCanFocused(false);
}


void TreeItemExpandButton::UpdateStyle() {

    __super::UpdateStyle();

    SetBorderColor(Color::Transparent());
    SetBackgroundColor(Color::Transparent());
}


void TreeItemExpandButton::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const {

    __super::Paint(canvas, dirty_rect);

    if (expand_state_ == ExpandState::None) {
        return;
    }

    const float buttom_edge_length = 8.f;
    const float rotate_angle = expand_state_ == ExpandState::Expanded ? 180.f : 90.f;
    auto triangle_geometry = internal::CreateTriangleGeometry(
        Width(), 
        Height(),
        buttom_edge_length, 
        rotate_angle);

    auto state_guard = canvas.PushState();

    auto color = IsSelectedInContext() ? Color::White() : Color::Black();
    canvas.SetBrushWithColor(color);
    canvas.DrawGeometry(triangle_geometry);
}


void TreeItemExpandButton::SetExpandState(ExpandState state) {

    expand_state_ = state;

    SetIsVisible(expand_state_ != ExpandState::None);
    NeedRepaint();
}

}