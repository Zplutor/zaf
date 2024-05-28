#include <zaf/control/color_picker.h>

namespace zaf {

ColorPicker CreateColorPicker(const Color& color) {
    return ConstantColorPicker(color);
}

}