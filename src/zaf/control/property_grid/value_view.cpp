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
    this->SetBackgroundColor(Color::White());

    this->SetBorder(Frame{ 0, 1, 1, 1});
}


void ValueView::UpdateVisualState() {

    __super::UpdateVisualState();

    SetBorderColor([this]() {

        auto parent = Parent();
        while (parent) {

            auto list_item = As<ListItem>(parent);
            if (list_item) {
                return list_item->BackgroundColor();
            }

            parent = parent->Parent();
        }

        return Color::White();
    }());
}


void ValueView::RecoverFocus() {

}

}