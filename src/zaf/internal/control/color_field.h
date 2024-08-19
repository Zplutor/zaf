#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/style/color_picker.h>
#include <zaf/graphic/color.h>

namespace zaf::internal {

class ColorField : NonCopyableNonMovable {
public:
    const zaf::Color& Color() const noexcept {
        return color_;
    }

    void SetColor(const zaf::Color& color, Control& owner);

    const zaf::ColorPicker& ColorPicker() const noexcept {
        return color_picker_;
    }

    void SetColorPicker(zaf::ColorPicker color_picker, Control& owner);

    void UpdateColor(const Control& owner);

private:
    zaf::Color color_;
    zaf::ColorPicker color_picker_;
};

}