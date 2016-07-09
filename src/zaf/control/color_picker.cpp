#include <zaf/control/color_picker.h>
#include <zaf/graphic/color.h>

namespace zaf {

const ColorPicker EmptyColorPicker = [](const Control&) { return Color::Transparent; };

}