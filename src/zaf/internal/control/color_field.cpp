#include <zaf/internal/control/color_field.h>
#include <zaf/control/control.h>

namespace zaf::internal {

void ColorField::SetColor(const zaf::Color& color, Control& owner) {

    color_ = color;
    color_picker_ = nullptr;

    owner.NeedRepaint();
}


void ColorField::SetColorPicker(zaf::ColorPicker color_picker, Control& owner) {

    color_picker_ = std::move(color_picker);
    UpdateColor(owner);

    owner.NeedRepaint();
}


void ColorField::UpdateColor(const Control& owner) {

    if (color_picker_) {
        color_ = color_picker_(owner);
    }
}

}