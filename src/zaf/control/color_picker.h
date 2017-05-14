#pragma once

#include <functional>
#include <zaf/graphic/color.h>

namespace zaf {

class Control;

/**
 The prorotype of color picker.

 @param control
    The control that getting the color.

 A color picker is called by controls to get colors.
 */
typedef std::function<Color(const Control& control)> ColorPicker;

/**
 Create a color picker which returns the specified color.

 This is a convenient function to create a simple color picker.
 */
inline ColorPicker CreateColorPicker(const Color& color) {
    return [color](const Control&) { return color; };
}

}