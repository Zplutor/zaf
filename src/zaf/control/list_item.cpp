#include <zaf/control/list_item.h>
#include <zaf/base/as.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>

namespace zaf {

ZAF_OBJECT_IMPL(ListItem);

void ListItem::Initialize() {

    __super::Initialize();

    SetCanFocus(true);

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

    item_data_ = std::move(data);
    OnItemDataChanged();
}


void ListItem::OnItemDataChanged() {

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

}