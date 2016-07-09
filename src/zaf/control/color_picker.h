#pragma once

#include <functional>
#include <zaf/graphic/color.h>

namespace zaf {

class Control;

typedef std::function<const Color(const Control& control)> ColorPicker;

inline const ColorPicker CreateColorPicker(const Color& color) {
    return [color](const Control&) { return color; };
}

}