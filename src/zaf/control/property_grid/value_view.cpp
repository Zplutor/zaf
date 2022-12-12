#include <zaf/control/property_grid/value_view.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>
#include <zaf/window/window.h>

namespace zaf::property_grid {

void ValueView::Initialize() {

    __super::Initialize();

    this->SetLayouter(Create<HorizontalLayouter>());
    this->SetBackgroundColor(Color::White());

    this->SetBorder(Frame{ 0, 1, 1, 1});
    this->SetBorderColorPicker([](const Control& control) {
    
        const auto& view = dynamic_cast<const ValueView&>(control);

        if (!view.IsSelected()) {
            return Color::White();
        }

        return Color::FromRGB(
            view.IsWithinFocusedControl() ? 
            internal::ControlSelectedActivedColorRGB :
            internal::ControlSelectedInActivedColorRGB);
    });
}


//TODO: duplicated codes with ListItem.
bool ValueView::IsWithinFocusedControl() const {

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