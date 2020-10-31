#include <zaf/control/tree_item_expand_button.h>
#include <zaf/control/internal/triangle_geometry.h>
#include <zaf/graphic/canvas.h>

namespace zaf {

void TreeItemExpandButton::Initialize() {

    __super::Initialize();

    SetCanFocused(false);

    SetBorderColor(Color::Transparent());
    SetBackgroundColor(Color::Transparent());
}


void TreeItemExpandButton::Paint(Canvas& canvas, const Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    if (expand_state_ == ExpandState::None) {
        return;
    }

    const float buttom_edge_length = 8.f;
    const float rotate_angle = expand_state_ == ExpandState::Expanded ? 180.f : 90.f;
    auto triangle_geometry = internal::CreateTriangleGeometry(
        GetWidth(), 
        GetHeight(),
        buttom_edge_length, 
        rotate_angle);

    Canvas::StateGuard guard(canvas);
    canvas.SetBrushWithColor(Color::Black());
    canvas.DrawGeometry(triangle_geometry);
}


void TreeItemExpandButton::SetExpandState(ExpandState state) {

    expand_state_ = state;

    SetIsVisible(expand_state_ != ExpandState::None);
    NeedRepaint();
}

}