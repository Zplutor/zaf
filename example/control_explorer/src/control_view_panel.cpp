#include "control_view_panel.h"

void ControlViewPanel::SetExploredControl(const std::shared_ptr<zaf::Control>& control) {

    if (explored_control_ != nullptr) {
        RemoveChild(explored_control_);
        explored_control_subscription_.Dispose();
    }

    explored_control_ = control;

    if (explored_control_ != nullptr) {
        explored_control_subscription_ = explored_control_->RectChangedEvent().Subscribe(
            std::bind(&ControlViewPanel::ExploredControlRectChanged, this, std::placeholders::_1));

        AddChild(explored_control_);
    }
}


void ControlViewPanel::ExploredControlRectChanged(const zaf::RectChangedInfo& event_info) {

    if (event_info.Source() != explored_control_) {
        return;
    }

    if (event_info.PreviousRect().size == explored_control_->Size()) {
        return;
    }

    NeedRelayout();
}


void ControlViewPanel::Layout(const zaf::Rect&) {

    if (explored_control_ == nullptr) {
        return;
    }

    auto rect = ContentRect();
    auto child_rect = explored_control_->Rect();

    zaf::Point child_position;
    child_position.x = (rect.size.width - child_rect.size.width) / 2;
    child_position.y = (rect.size.height - child_rect.size.height) / 2;
    explored_control_->SetPosition(child_position);
}