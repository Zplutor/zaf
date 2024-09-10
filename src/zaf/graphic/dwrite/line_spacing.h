#pragma once

#include <dwrite.h>

namespace zaf::dwrite {

/**
The method used for line spacing.
*/
enum class LineSpacingMethod {

    /**
    Line spacing depends solely on the content, adjusting to accommodate the size of fonts and 
    inline objects.
    */
    Default = DWRITE_LINE_SPACING_METHOD_DEFAULT,

    /**
    Lines are explicitly set to uniform spacing, regardless of the size of fonts and inline
    objects.
    */
    Uniform = DWRITE_LINE_SPACING_METHOD_UNIFORM,
};

/**
Describes the line spacing settings for a text layout.
*/
class LineSpacing {
public:
    /**
    Constructs a default line spacing.
    */
    LineSpacing() noexcept = default;

    /**
    Constructs a uniform line spacing with the specified heights.
    */
    LineSpacing(float line_height, float height_above_baseline) noexcept :
        method_(LineSpacingMethod::Uniform),
        line_height_(line_height),
        height_above_baseline_(height_above_baseline) {

    }

    /**
    Constructs a line spacing with the specified properties.
    */
    LineSpacing(LineSpacingMethod method, float line_height, float height_above_baseline) noexcept
        :
        method_(method),
        line_height_(line_height),
        height_above_baseline_(height_above_baseline) {

    }

    LineSpacingMethod Method() const noexcept {
        return method_;
    }

    void SetMethod(LineSpacingMethod method) noexcept {
        method_ = method;
    }

    float LineHeight() const noexcept {
        return line_height_;
    }

    void SetLineHeight(float value) noexcept {
        line_height_ = value;
    }

    float HeightAboveBaseline() const noexcept {
        return height_above_baseline_;
    }

    void SetHeightAboveBaseline(float value) noexcept {
        height_above_baseline_ = value;
    }

private:
    LineSpacingMethod method_{ LineSpacingMethod::Default };
    float line_height_{};
    float height_above_baseline_{};
};

}