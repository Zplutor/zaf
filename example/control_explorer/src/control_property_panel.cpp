#include "control_property_panel.h"
#include <zaf/control/layout/array_layouter.h>
#include <zaf/creation.h>
#include "property/property_item.h"

static void PropertyPanelLayouter(
    const std::shared_ptr<zaf::Control>& parent,
    const zaf::Rect& preivous_rect,
    const std::vector<std::shared_ptr<zaf::Control>>& children);

void ControlPropertyPanel::Initialize() {

    __super::Initialize();

    SetLayouter(zaf::GetHorizontalArrayLayouter());

    scrollable_control_ = zaf::Create<zaf::ScrollableControl>();
    scrollable_control_->SetBorder(0);
    scrollable_control_->SetAllowHorizontalScroll(false);
    scrollable_control_->GetScrollContentControl()->SetLayouter(PropertyPanelLayouter);
    AddChild(scrollable_control_);
}


void ControlPropertyPanel::SetPropertyItems(const std::vector<std::shared_ptr<PropertyItem>>& items) {

    auto scroll_content_control = scrollable_control_->GetScrollContentControl();
    scroll_content_control->RemoveAllChildren();

    UpdateGuard update_guard(*scroll_content_control);

    float total_height = 0;
    for (const auto& each_item : items) {

        scroll_content_control->AddChild(each_item);
        total_height += each_item->GetHeight();
    }
    scrollable_control_->SetScrollContentSize(zaf::Size(0, total_height));
}


static void PropertyPanelLayouter(
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