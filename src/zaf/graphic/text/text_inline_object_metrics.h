#pragma once

#include <dwrite.h>

namespace zaf {

class TextInlineObjectMetrics : private DWRITE_INLINE_OBJECT_METRICS {
public:
    float Width() const {
        return this->width;
    }

    void SetWidth(float width) {
        this->width = width;
    }

    float Height() const {
        return this->height;
    }

    void SetHeight(float height) {
        this->height = height;
    }

    float Baseline() const {
        return this->baseline;
    }

    void SetBaseline(float baseline) {
        this->baseline = baseline;
    }

    const DWRITE_INLINE_OBJECT_METRICS& Inner() const {
        return *this;
    }
};

static_assert(sizeof(TextInlineObjectMetrics) == sizeof(DWRITE_INLINE_OBJECT_METRICS));

}