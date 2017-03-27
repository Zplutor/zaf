#include "control_operate_panel.h"
#include <zaf/control/layout/array_layouter.h>
#include <zaf/creation.h>

static void OperatePanelLayouter(
    const std::shared_ptr<zaf::Control>& parent,
    const zaf::Rect& preivous_rect,
    const std::vector<std::shared_ptr<zaf::Control>>& children);

void ControlOperatePanel::Initialize() {

    __super::Initialize();

    SetLayouter(zaf::GetHorizontalArrayLayouter());

    scrollable_control_ = zaf::Create<zaf::ScrollableControl>();
    scrollable_control_->SetBorder(0);
    scrollable_control_->SetAllowHorizontalScroll(false);
    scrollable_control_->GetScrollContentControl()->SetLayouter(OperatePanelLayouter);
    AddChild(scrollable_control_);
}


void ControlOperatePanel::SetOperatePanels(const std::vector<std::shared_ptr<zaf::Control>>& panels) {

    auto scroll_content_control = scrollable_control_->GetScrollContentControl();
    scroll_content_control->RemoveAllChildren();

    float total_height = 0;
    for (const auto& each_panel : panels) {
        total_height += each_panel->GetHeight();
    }
    scrollable_control_->SetScrollContentSize(zaf::Size(0, total_height));

    scroll_content_control->AddChildren(panels);
}


static void OperatePanelLayouter(
    const std::shared_ptr<zaf::Control>& parent,
    const zaf::Rect& preivous_rect,
    const std::vector<std::shared_ptr<zaf::Control>>& children) {

    float y = 0;
    auto width = parent->GetContentSize().width;

    for (const auto& each_child : children) {

        auto child_rect = each_child->GetRect();
        child_rect.position.x = 0;
        child_rect.position.y = y;
        child_rect.size.width = width;

        each_child->SetRect(child_rect);

        y += child_rect.size.height;
    }
}