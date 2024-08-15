#include <zaf/control/property_grid/value_view.h>
#include <zaf/base/as.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/control/list_item.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>

namespace zaf::property_grid {

void ValueView::Initialize() {

    __super::Initialize();

    this->SetLayouter(Create<HorizontalLayouter>());
    this->SetBorder(Frame{ 0, 1, 1, 1});

    SetBackgroundColor(Color::White());

    SetBorderColorPicker(ColorPicker([this](const Control& control) {

        auto parent = Parent();
        while (parent) {

            auto list_item = As<ListItem>(parent);
            if (list_item) {
                return list_item->BackgroundColor();
            }

            parent = parent->Parent();
        }

        return Color::White();
    }));
}


void ValueView::RecoverFocus() {

}

}