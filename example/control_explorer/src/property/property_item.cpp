#include "property/property_item.h"
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/creation.h>

namespace {

const float TitleWidth = 200;
const float DefaultItemHeight = 30;

}

void ValuePropertyItem::Initialize() {

    __super::Initialize();

    SetHeight(DefaultItemHeight);
    SetPadding(zaf::Frame(5, 5, 5, 5));

    title_control_ = zaf::Create<zaf::Label>();
    title_control_->SetParagraphAlignment(zaf::ParagraphAlignment::Center);
    AddChild(title_control_);
}


void ValuePropertyItem::Layout(const zaf::Rect&) {

    auto content_rect = GetContentRect();

    title_control_->SetRect(zaf::Rect(0, 0, TitleWidth, content_rect.size.height));

    if (value_control_ != nullptr) {
        value_control_->SetRect(zaf::Rect(TitleWidth, 0, content_rect.size.width - TitleWidth, content_rect.size.height));
    }
}


void ValuePropertyItem::SetValueControl(const std::shared_ptr<zaf::Control>& control) {

    if (value_control_ != nullptr) {
        RemoveChild(value_control_);
    }

    value_control_ = control;

    if (control != nullptr) {
        AddChild(value_control_);
    }
}


void GroupPropertyItem::Initialize() {

    __super::Initialize();

    SetLayouter(zaf::Create<zaf::VerticalLayouter>());
}


void GroupPropertyItem::AddItem(const std::shared_ptr<PropertyItem>& item) {

    AddChild(item);
    SetHeight(Height() + item->Height());
}


std::shared_ptr<PropertyItem> CreatePropertyItem(
    const std::wstring& title,
    const std::shared_ptr<zaf::Control>& value_control) {

    auto property_item = zaf::Create<ValuePropertyItem>();
    property_item->SetTitle(title);
    property_item->SetValueControl(value_control);
    return property_item;
}


std::shared_ptr<PropertyItem> CreatePropertyItem(const std::vector<std::shared_ptr<PropertyItem>>& items) {

    auto property_item = zaf::Create<GroupPropertyItem>();
    for (const auto& each_item : items) {
        property_item->AddItem(each_item);
    }

    return property_item;
}