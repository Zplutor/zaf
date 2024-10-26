#include <zaf/control/list_item.h>
#include <zaf/base/as.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/control/list_control.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>
#include <zaf/window/window.h>

namespace zaf {

void ListItem::Initialize() {

    __super::Initialize();

    SetCanFocused(true);
    SetParagraphAlignment(dwrite::ParagraphAlignment::Center);
    SetPadding(Frame(2, 0, 2, 0));

    SetTextColorPicker(ColorPicker([](const Control& control) {
        return control.IsSelectedInContext() ?
            Color::White() :
            Color::FromRGB(internal::ControlNormalTextColorRGB);
    }));

    SetBackgroundColorPicker(ColorPicker([](const Control& control) {

        const auto& list_item = As<ListItem>(control);

        if (!list_item.IsSelectedInContext()) {
            return Color::Transparent();
        }

        return Color::FromRGB(
            list_item.IsInFocusContext() ?
            internal::ControlSelectedActivedColorRGB :
            internal::ControlSelectedInActivedColorRGB);
    }));
}


void ListItem::OnFocusGained(const FocusGainedInfo& event_info) {
    __super::OnFocusGained(event_info);
    NeedRepaint();
}


void ListItem::OnFocusLost(const FocusLostInfo& event_info) {
    __super::OnFocusLost(event_info);
    NeedRepaint();
}


bool ListItem::IsInFocusContext() const {

    for (auto parent = Parent(); parent; parent = parent->Parent()) {

        auto list_control = As<ListControl>(parent);
        if (list_control) {
            return list_control->ContainsFocus();
        }
    }

    return false;
}


void ListItem::RecoverFocus() {

}

}