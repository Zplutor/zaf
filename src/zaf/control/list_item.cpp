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


bool ListItem::IsInFocusContext() const {

    auto container = As<ListItemContainer>(Parent());
    if (container) {
        return container->ContainsFocus();
    }
    return false;
}


void ListItem::RecoverFocus() {

}

}