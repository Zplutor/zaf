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

    friend bool operator==(const ColorPicker&, const ColorPicker&) noexcept;

private:
    ColorPickerFunction function_;
};


class StaticColorPicker : public Object {
public:
    ZAF_DECLARE_TYPE;

public:
    explicit StaticColorPicker(const zaf::Color& color) noexcept : color_(color) {

    }

    const zaf::Color& Color() const noexcept {
        return color_;
    }

    zaf::Color operator()(const Control&) {
        return color_;
    }

    std::size_t Hash() const override {
        return color_.Hash();
    }

    bool IsEqual(const Object& other) const override;

private:
    zaf::Color color_;
};


/**
 Create a constant color picker that returns the specified color.
 */
ColorPicker CreateColorPicker(const Color& color);

}