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


void ListItem::SetItemData(std::shared_ptr<Object> data) {

    ZAF_EXPECT(data);

    auto previous_data = std::move(item_data_);
    item_data_ = std::move(data);

    OnItemDataChanged(previous_data);
}


void ListItem::OnItemDataChanged(const std::shared_ptr<Object>& previous_data) {

}


bool ListItem::IsInFocusContext() const {

    auto item_container = Parent();
    if (!item_container) {
        return false;
    }

    auto viewport_container = item_container->Parent();
    if (!viewport_container) {
        return false;
    }

    auto scroll_box = viewport_container->Parent();
    if (!scroll_box) {
        return false;
    }

    return scroll_box->ContainsFocus();
}


void ListItem::RecoverFocus() {

}

}