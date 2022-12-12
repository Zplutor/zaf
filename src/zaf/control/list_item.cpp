#include <zaf/control/list_item.h>
#include <zaf/control/layout/linear_layouter.h>
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
            item.IsWithinFocusedControl() ? 
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


bool ListItem::IsWithinFocusedControl() const {

    auto window = this->Window();
    if (!window) {
        return false;
    }

    auto focused_control = window->FocusedControl();
    if (!focused_control) {
        return false;
    }

    return 
        this->IsAncestorOf(focused_control) ||
        focused_control->IsAncestorOf(shared_from_this());
}

}