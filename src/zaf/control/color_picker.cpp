#include <zaf/control/color_picker.h>
#include <zaf/base/as.h>
#include <zaf/object/type_definition.h>

namespace zaf {

bool operator==(const ColorPicker& picker1, const ColorPicker& picker2) noexcept {

    auto picker_object1 = picker1.function_.target<StaticColorPicker>();
    if (!picker_object1) {
        return false;
    }

    auto picker_object2 = picker2.function_.target<Object>();
    if (!picker_object2) {
        return false;
    }

    return picker_object1->IsEqual(*picker_object2);
}


ZAF_DEFINE_TYPE(StaticColorPicker)
ZAF_DEFINE_TYPE_END;


ColorPicker CreateColorPicker(const Color& color) {
    return StaticColorPicker{ color };
}


bool StaticColorPicker::IsEqual(const Object& other) const {

    auto static_color_picker = As<StaticColorPicker>(&other);
    if (static_color_picker) {
        return this->Color() == static_color_picker->Color();
    }
    return false;
}

}