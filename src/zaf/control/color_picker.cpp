#include <zaf/control/color_picker.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(ColorPickerObject)
ZAF_DEFINE_TYPE_END;

ZAF_DEFINE_TYPE(StaticColorPicker)
ZAF_DEFINE_TYPE_END;

ColorPicker CreateColorPicker(const Color& color) {
    return ConstantColorPicker(color);
}

}