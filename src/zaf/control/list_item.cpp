#include <zaf/control/list_item.h>
#include <zaf/base/as.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/control/list_item_container.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>
#include <zaf/window/window.h>

namespace zaf {

void ListItem::Initialize() {

    __super::Initialize();

    SetParagraphAlignment(ParagraphAlignment::Center);
    SetPadding(Frame(2, 0, 2, 0));
}


void ListItem::UpdateStyle() {

    __super::UpdateStyle();

    SetTextColor(
        IsSelectedInContext() ?
        Color::White() :
        Color::FromRGB(internal::ControlNormalTextColorRGB));

    SetBackgroundColor([this]() {

        if (!IsSelectedInContext()) {
            return Color::Transparent();
        }

        return Color::FromRGB(
            IsInFocusContext() ?
            internal::ControlSelectedActivedColorRGB :
            internal::ControlSelectedInActivedColorRGB);
    }());
}


bool ListItem::IsInFocusContext() const {

    auto container = As<ListItemContainer>(Parent());
    if (container) {
        return container->ContainFocus();
    }
    return false;
}


void ListItem::RecoverFocus() {

}

}