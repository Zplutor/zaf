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
        if (!item.IsSelected()) {
            return Color::Transparent();
        }

        return Color::FromRGB(
            item.IsWithinFocusedContext() ? 
            internal::ControlSelectedActivedColorRGB :
            internal::ControlSelectedInActivedColorRGB);
    });

    SetTextColorPicker([](const Control& control) {

        const auto& item = dynamic_cast<const ListItem&>(control);
        if (item.IsSelected()) {
            return Color::White();
        }
        else {
            return Color::FromRGB(internal::ControlNormalTextColorRGB);
        }
    });
}


bool ListItem::IsWithinFocusedContext() const {

    auto window = this->Window();
    if (!window) {
        return false;
    }

    auto focused_control = window->FocusedControl();
    if (!focused_control) {
        return false;
    }

    auto container = As<ListItemContainer>(Parent());
    if (!container) {
        return false;
    }

    return 
        container == focused_control ||
        container->IsAncestorOf(focused_control);
}

}