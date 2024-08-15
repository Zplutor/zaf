#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/style/color_picker.h>
#include <zaf/graphic/color.h>

namespace zaf::internal {

class ColorField : NonCopyableNonMovable {
public:
    explicit ColorField(Control* owner) : owner_(owner) { 
    
    }

    const zaf::Color& Color() const {
        return color_;
    }

    void SetColor(const zaf::Color& color);

    const zaf::ColorPicker& ColorPicker() const {
        return color_picker_;
    }

    void SetColorPicker(zaf::ColorPicker color_picker);

    void UpdateColor();

private:
    Control* owner_{};
    zaf::Color color_;
    zaf::ColorPicker color_picker_;
};

}