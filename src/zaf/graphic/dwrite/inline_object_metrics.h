#pragma once

#include <dwrite.h>

namespace zaf::dwrite {

/**
Contains properties describing the geometric measurement of an inline object.
*/
class InlineObjectMetrics : private DWRITE_INLINE_OBJECT_METRICS {
public:
    InlineObjectMetrics() : DWRITE_INLINE_OBJECT_METRICS{} {

    }

    /**
    Gets the width of the object.

    @details
        The default value is 0.
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

    @details
        The default value is 0.
    */
    float Height() const {
        return this->height;
    }

    /**
    Sets the height of the object.

    @details
        After setting the height, the baseline should also be adjusted using SetBaseline. Or users
        can use SetHeightAndBaseline to set the height and adjust the baseline to a typical value 
        automatically.
    */
    void SetHeight(float height) {
        this->height = height;
    }

    /**
    Gets the baseline, which is the height from the top of the object to the baseline of its 
    adjacent text.

    @details
        The default value is 0.
    */
    float Baseline() const {
        return this->baseline;
    }

    /**
    Sets the baseline, which is the height from the top of the object to the baseline of its 
    adjacent text.

    @details
        Users can change the vertical position of the object by setting the baseline. A typical 
        value is 0.8 times the height of the object.
    */
    void SetBaseline(float height) {
        this->baseline = height;
    }

    /**
    Sets the height of the object and automatically adjusts the baseline to 0.8 times the height.
    */
    void SetHeightAndBaseline(float height) {
        SetHeight(height);
        SetBaseline(height * 0.8f);
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