#pragma once

#include <functional>
#include <zaf/graphic/color.h>
#include <zaf/object/object.h>

namespace zaf {

class Control;

using ColorPickerFunction = std::function<Color(const Control&)>;

class ColorPicker {
public:
    ColorPicker() = default;

    ColorPicker(const ColorPicker&) = default;
    ColorPicker& operator=(const ColorPicker&) = default;

    ColorPicker(ColorPicker&&) = default;
    ColorPicker& operator=(ColorPicker&&) = default;

    template<
        typename Callable, 
        typename = std::enable_if_t<std::is_convertible_v<Callable, ColorPickerFunction>>
    >
    ColorPicker(Callable&& function) : function_(std::forward<Callable>(function)) {
    
    }

    template<
        typename Callable,
        typename = std::enable_if_t<std::is_convertible_v<Callable, ColorPickerFunction>>
    >
    ColorPicker& operator=(Callable&& function) {
        function_ = std::forward<Callable>(function);
        return *this;
    }

    Color operator()(const Control& control) const {
        return function_(control);
    }

    explicit operator bool() const noexcept {
        return !!function_;
    }

private:
    ColorPickerFunction function_;
};


class ColorPickerObject : public Object {
public:
    ZAF_DECLARE_TYPE;
};


class StaticColorPicker : public ColorPickerObject {
public:
    ZAF_DECLARE_TYPE;

    Color operator()(const Control&) {
        return color_;
    }

private:
    Color color_;
};


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