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

    SetBackgroundColorPicker([](const Control& control) {

        const auto& item = dynamic_cast<const ListItem&>(control);
        if (!item.IsSelectedInContext()) {
            return Color::Transparent();
        }

        return Color::FromRGB(
            item.IsInFocusContext() ?
            internal::ControlSelectedActivedColorRGB :
            internal::ControlSelectedInActivedColorRGB);
    });
}


void ListItem::UpdateVisualState() {

    __super::UpdateVisualState();

    SetTextColor(
        IsSelectedInContext() ?
        Color::White() :
        Color::FromRGB(internal::ControlNormalTextColorRGB));
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