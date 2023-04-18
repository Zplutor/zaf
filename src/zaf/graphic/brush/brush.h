#pragma once

#include <zaf/base/com_object.h>
#include <zaf/base/direct2d.h>

namespace zaf {

/**
 Represents a brush.

 This is a base class of all concrete brush.
 */
class Brush : public COMObject<ID2D1Brush> {
public:
    Brush() { }
    explicit Brush(ID2D1Brush* handle) : COMObject(handle) { }

    /**
     Get opacity.
     */
    float GetOpacity() const {
        return GetHandle()->GetOpacity();
    }

    /**
     Set opacity.
     */
    void SetOpacity(float opacity) {
        GetHandle()->SetOpacity(opacity);
    }
};

}