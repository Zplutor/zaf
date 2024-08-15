#include <zaf/internal/control/color_field.h>
#include <zaf/control/control.h>

namespace zaf::internal {

void ColorField::SetColor(const zaf::Color& color) {

    color_ = color;
    color_picker_ = nullptr;

    owner_->NeedRepaint();
}


void ColorField::SetColorPicker(zaf::ColorPicker color_picker) {

    color_picker_ = std::move(color_picker);
    UpdateColor();

    owner_->NeedRepaint();
}


void ColorField::UpdateColor() {

    if (color_picker_) {
        color_ = color_picker_(*owner_);
    }
}

}