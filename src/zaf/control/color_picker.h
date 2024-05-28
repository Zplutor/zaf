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
 Represents a color picker that has constant color.

 Typically, you should use CreateColorPicker function to create a 
 constant color picker.
 */
class ConstantColorPicker {
public:
    ConstantColorPicker() { }
    ConstantColorPicker(const Color& color) : color_(color) { }

    Color operator()(const Control&) {
        return GetColor();
    }

    const Color& GetColor() const {
        return color_;
    }

private:
    Color color_;
};

/**
 Create a constant color picker that returns the specified color.
 */
ColorPicker CreateColorPicker(const Color& color);

}