#include "control_view_panel.h"

void ControlViewPanel::Layout(const zaf::Rect&) {

    const auto& children = GetChildren();
    if (children.empty()) {
        return;
    }

    const auto& child = children.front();

    auto rect = GetContentRect();
    auto child_rect = child->GetRect();

    zaf::Point child_position;
    child_position.x = (rect.size.width - child_rect.size.width) / 2;
    child_position.y = (rect.size.height - child_rect.size.height) / 2;
    child->SetPosition(child_position);
}