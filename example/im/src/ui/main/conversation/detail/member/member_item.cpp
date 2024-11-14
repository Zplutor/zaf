#include "ui/main/conversation/detail/member/member_item.h"
#include <zaf/graphic/canvas.h>

static const float IconSize = 8;

void MemberItem::Initialize() {

    __super::Initialize();

    SetPadding(zaf::Frame(10, 0, 0, 0));

    label_ = zaf::Create<zaf::Label>();
    label_->SetTextColor(zaf::Color::Black());
    label_->SetParagraphAlignment(zaf::dwrite::ParagraphAlignment::Center);
    this->AddChild(label_);

    SetBackgroundColorPicker(zaf::ColorPicker([](const zaf::Control& control) {
    
        if (control.IsSelectedInContext()) {
            return zaf::Color::FromRGB(0xAED2F6);
        }
        else if (control.IsMouseOver()) {
            return zaf::Color::FromRGB(0xE0E9F2);
        }
        else {
            return zaf::Color::FromRGB(0xECF0F3);
        }
    }));
}


void MemberItem::Layout(const zaf::Rect& old_rect) {

    __super::Layout(old_rect);

    zaf::Rect label_rect{ {}, ContentSize() };
    label_rect.Deflate(zaf::Frame(IconSize + 5, 0, 0, 0));
    label_->SetRect(label_rect);
}


void MemberItem::Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) const {

    __super::Paint(canvas, dirty_rect);

    auto content_rect = ContentRect();

    float radius = IconSize / 2;

    zaf::Ellipse ellipse(
        zaf::Point(content_rect.position.x + radius, content_rect.size.height / 2),
        radius,
        radius);

    std::uint32_t color_rgb = gender_ == User::Gender::Female ? 0xFDA357 : 0x7CA3D2;
    canvas.SetBrushWithColor(zaf::Color::FromRGB(color_rgb));
    canvas.DrawEllipse(ellipse);
}

