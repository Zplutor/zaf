#include <zaf/control/list_item.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>

namespace zaf {

void ListItem::Initialize() {

    __super::Initialize();

    SetPadding(Frame(2, 0, 2, 0));

    SetBackgroundColorPicker([](const Control& control) {

        const auto& item = dynamic_cast<const ListItem&>(control);
        if (item.IsSelected()) {
            return Color::FromRGB(internal::ControlSelectedColorRGB);
        }
        else {
            return Color::Transparent();
        }
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

}