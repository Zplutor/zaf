#pragma once

#include <zaf/base/com_ptr.h>
#include <zaf/base/direct2d.h>

namespace zaf {

/**
 Represents a brush.

 This is a base class of all concrete brush.
 */
class Brush : public COMPtr<ID2D1Brush> {
public:
    Brush() { }
    explicit Brush(ID2D1Brush* handle) : COMPtr(handle) { }

    /**
     Get opacity.
     */
    float GetOpacity() const {
        return Inner()->GetOpacity();
    }

    /**
     Set opacity.
     */
    void SetOpacity(float opacity) {
        Inner()->SetOpacity(opacity);
    }
};

}