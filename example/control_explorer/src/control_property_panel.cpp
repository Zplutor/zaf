#include "control_property_panel.h"
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/creation.h>
#include "property/property_item.h"

namespace {

class PropertyPanelLayouter : public zaf::Layouter {
public:
    virtual void Layout(
        const zaf::Control& parent,
        const zaf::Rect& parent_old_rect,
        const std::vector<std::shared_ptr<zaf::Control>>& children) override {

        float y = 0;
        auto width = parent.GetContentSize().width;

        for (const auto& each_child : children) {

            auto child_rect = each_child->Rect();
            child_rect.position.x = 0;
            child_rect.position.y = y;
            child_rect.size.width = width;

            each_child->SetRect(child_rect);

            y += child_rect.size.height;
        }
    }
};

}


void ControlPropertyPanel::Initialize() {

    __super::Initialize();

    SetLayouter(zaf::Create<zaf::HorizontalLayouter>());

    scrollable_control_ = zaf::Create<zaf::ScrollableControl>();
    scrollable_control_->SetBorder(0);
    scrollable_control_->SetAllowHorizontalScroll(false);

    auto content_control = scrollable_control_->GetScrollContentControl();
    content_control->SetIsCachedPaintingEnabled(false);
    content_control->SetLayouter(
        zaf::Create<PropertyPanelLayouter>());

    AddChild(scrollable_control_);
}


void ControlPropertyPanel::SetPropertyItems(const std::vector<std::shared_ptr<PropertyItem>>& items) {

    auto scroll_content_control = scrollable_control_->GetScrollContentControl();
    scroll_content_control->RemoveAllChildren();

    auto update_guard = scroll_content_control->BeginUpdate();

    float total_height = 0;
    for (const auto& each_item : items) {

        scroll_content_control->AddChild(each_item);
        total_height += each_item->Height();
    }
    scrollable_control_->SetScrollContentSize(zaf::Size(0, total_height));
}