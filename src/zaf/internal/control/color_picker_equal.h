#pragma once

#include <zaf/control/style/color_picker.h>

namespace zaf::internal {

struct ColorPickerEqual {
    bool operator()(const ColorPicker&, const ColorPicker&) const {
        return false;
    }
};

}