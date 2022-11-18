#include <zaf/control/property_grid/value_view.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>

namespace zaf::property_grid {

void ValueView::Initialize() {

    __super::Initialize();

    this->SetLayouter(Create<HorizontalLayouter>());
    this->SetBackgroundColor(Color::White());

    this->SetBorder(Frame{ 0, 1, 1, 1});
    this->SetBorderColorPicker([](const Control& control) {
    
        if (control.IsSelected()) {
            return Color::FromRGB(internal::ControlSelectedColorRGB);
        }
        return Color::White();
    });
}

}