#include "operate/operate_item.h"
#include <zaf/creation.h>
#include <zaf/control/layout/array_layouter.h>

static const float NameWidth = 150;
static const float DefaultItemHeight = 30;

void OperateItem::Initialize() {

    __super::Initialize();

    SetItemHeight(DefaultItemHeight);

    SetPadding(zaf::Frame(5, 5, 5, 5));

    name_label_ = zaf::Create<zaf::Label>();
    name_label_->SetParagraphAlignment(zaf::ParagraphAlignment::Center);
    AddChild(name_label_);
}


void OperateItem::Layout(const zaf::Rect&) {

    auto content_rect = GetContentRect();

    name_label_->SetRect(zaf::Rect(0, 0, NameWidth, content_rect.size.height));

    if (value_control_ != nullptr) {
        value_control_->SetRect(zaf::Rect(NameWidth, 0, content_rect.size.width - NameWidth, content_rect.size.height));
    }
}


void OperateItem::SetTitle(const std::wstring& name, std::size_t ident_level) {
    name_label_->SetText(name);
}