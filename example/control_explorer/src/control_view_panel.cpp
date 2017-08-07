#include "control_view_panel.h"

ControlViewPanel::~ControlViewPanel() {

    if (explored_control_ != nullptr) {
        explored_control_->GetRectChangeEvent().RemoveListenersWithTag(reinterpret_cast<std::uintptr_t>(this));
    }
}


void ControlViewPanel::SetExploredControl(const std::shared_ptr<zaf::Control>& control) {

    if (explored_control_ != nullptr) {
        explored_control_->GetRectChangeEvent().RemoveListenersWithTag(reinterpret_cast<std::uintptr_t>(this));
        RemoveChild(explored_control_);
    }

    explored_control_ = control;

    if (explored_control_ != nullptr) {
        explored_control_->GetRectChangeEvent().AddListenerWithTag(
            reinterpret_cast<std::uintptr_t>(this),
            std::bind(&ControlViewPanel::ExploredControlRectChanged, this, std::placeholders::_1, std::placeholders::_2));

        AddChild(explored_control_);
    }
}


void ControlViewPanel::ExploredControlRectChanged(
    const std::shared_ptr<zaf::Control>& control,
    const zaf::Rect& previous_rect) {

    if (control != explored_control_) {
        return;
    }

    if (previous_rect.size == explored_control_->GetSize()) {
        return;
    }

    NeedRelayout();
}


void ControlViewPanel::Layout(const zaf::Rect&) {

    if (explored_control_ == nullptr) {
        return;
    }

    auto rect = GetContentRect();
    auto child_rect = explored_control_->GetRect();

    zaf::Point child_position;
    child_position.x = (rect.size.width - child_rect.size.width) / 2;
    child_position.y = (rect.size.height - child_rect.size.height) / 2;
    explored_control_->SetPosition(child_position);
}