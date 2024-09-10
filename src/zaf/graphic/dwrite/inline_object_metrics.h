#pragma once

#include <dwrite.h>

namespace zaf::dwrite {

/**
Contains properties describing the geometric measurement of an inline object.
*/
class InlineObjectMetrics : private DWRITE_INLINE_OBJECT_METRICS {
public:
    /**
    Gets the width of the object.
    */
    float Width() const {
        return this->width;
    }

    /**
    Sets the width of the object.
    */
    void SetWidth(float width) {
        this->width = width;
    }

    /**
    Gets the height of the object.
    */
    float Height() const {
        return this->height;
    }

    /**
    Sets the height of the object.
    */
    void SetHeight(float height) {
        this->height = height;
    }

    /**
    Gets the height from the top of the object to the baseline of its adjacent text.
    */
    float HeightAboveBaseline() const {
        return this->baseline;
    }

    /**
    Sets the height from the top of the object to the baseline of its adjacent text.

    @remark
        You can change the vertical position of the object by setting the height above the 
        baseline. A typical value is 0.8 times the height of the object.
    */
    void SetHeightAboveBaseline(float height) {
        this->baseline = height;
    }

    /**
    Gets the underlying DWRITE_INLINE_OBJECT_METRICS structure.
    */
    const DWRITE_INLINE_OBJECT_METRICS& Inner() const {
        return *this;
    }
};

static_assert(sizeof(InlineObjectMetrics) == sizeof(DWRITE_INLINE_OBJECT_METRICS));

}