#pragma once

#include <functional>

namespace zaf {

class Color;
class Control;

typedef std::function<const Color(const Control& control)> ColorPicker;

extern const ColorPicker EmptyColorPicker;

}