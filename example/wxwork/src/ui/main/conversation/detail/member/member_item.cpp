#include "ui/main/conversation/detail/member/member_item.h"

void MemberItem::Initialize() {

    __super::Initialize();

    SetBackgroundColorPicker([this](const Control&) {
    
        if (IsSelected()) {
            return zaf::Color::FromRGB(0xAED2F6);
        }
        else if (IsHovered()) {
            return zaf::Color::FromRGB(0xE0E9F2);
        }
        else {
            return zaf::Color::FromRGB(0xECF0F3);
        }
    });

    SetDefaultTextColor(zaf::Color::Black);

    SetParagraphAlignment(zaf::ParagraphAlignment::Center);
}