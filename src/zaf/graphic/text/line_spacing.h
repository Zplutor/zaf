#pragma once

#include <dwrite.h>

namespace zaf {

enum class LineSpacingMethod {
    Default = DWRITE_LINE_SPACING_METHOD_DEFAULT,
    Uniform = DWRITE_LINE_SPACING_METHOD_UNIFORM,
};

class LineSpacing {
public:
    LineSpacing() = default;

    LineSpacing(LineSpacingMethod method, float line_height, float baseline) :
        method_(method),
        line_height_(line_height),
        baseline_(baseline) {

    }

    LineSpacingMethod Method() const {
        return method_;
    }

    void SetMethod(LineSpacingMethod method) {
        method_ = method;
    }

    float LineHeight() const {
        return line_height_;
    }

    void SetLineHeight(float value) {
        line_height_ = value;
    }

    float Baseline() const {
        return baseline_;
    }

    void SetBaseline(float value) {
        baseline_ = value;
    }

private:
    LineSpacingMethod method_{ LineSpacingMethod::Default };
    float line_height_{};
    float baseline_{};
};

}