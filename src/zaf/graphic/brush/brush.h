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
    using COMObject::COMObject;

    virtual ~Brush() = default;

    /**
     Get opacity.
     */
    float GetOpacity() const {
        return Ptr()->GetOpacity();
    }

    /**
     Set opacity.
     */
    void SetOpacity(float opacity) {
        Ptr()->SetOpacity(opacity);
    }
};

}