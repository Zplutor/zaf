#include <zaf/control/menu_item.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>

namespace zaf {

void MenuItem::Initialize() {

    __super::Initialize();

    /*
    SetLayouter(Create<HorizontalLayouter>());
    auto child = Create<Control>();
    child->SetBackgroundColor(zaf::Color::Red());
    child->SetFixedWidth(20);
    this->AddChild(child);
    */

    SetFixedHeight(30);
    SetPadding(Frame{ 8, 8, 8, 8 });
    SetTextAlignment(TextAlignment::Left);
    SetParagraphAlignment(ParagraphAlignment::Center);

    Subscriptions() += MouseEnterEvent().Subscribe([this](const MouseEnterInfo& event_info) {
        if (event_info.Sender().get() == this) {
            AdjustAppearence();
        }
    });

    Subscriptions() += MouseLeaveEvent().Subscribe([this](const MouseLeaveInfo& event_info) {
        if (event_info.Sender().get() == this) {
            AdjustAppearence();
        }
    });
}


void MenuItem::AdjustAppearence() {

    bool contain_mouse = ContainMouse();

    SetTextColor(contain_mouse ? zaf::Color::White() : zaf::Color::Black());
    SetBackgroundColor(
        contain_mouse ? 
        zaf::Color::FromRGB(internal::ControlSelectedActivedColorRGB) : 
        zaf::Color::White());
}

}