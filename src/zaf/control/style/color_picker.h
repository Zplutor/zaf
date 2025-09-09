#pragma once

#include <functional>
#include <zaf/control/style/color_picker_parser.h>
#include <zaf/graphic/color.h>
#include <zaf/dynamic/object.h>

namespace zaf {

class Control;

class ColorPicker : public Object {
public:
    ZAF_OBJECT;

    using Function = std::function<Color(const Control&)>;

public:
    ColorPicker() = default;

    ColorPicker(Function function) : function_(std::move(function)) {

    }

    ColorPicker(const ColorPicker&) = default;
    ColorPicker& operator=(const ColorPicker&) = default;

    ColorPicker& operator=(Function function) {
        function_ = std::move(function);
        return *this;
    }

    explicit operator bool() const {
        return static_cast<bool>(function_);
    }

    Color operator()(const Control& control) const {
        return function_(control);
    }

private:
    Function function_;
};

ZAF_OBJECT_BEGIN(ColorPicker);
ZAF_OBJECT_PARSER(ColorPickerParser);
ZAF_OBJECT_END;

}