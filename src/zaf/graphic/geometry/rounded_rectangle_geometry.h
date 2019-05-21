#pragma once

#include <zaf/graphic/geometry/geometry.h>
#include <zaf/graphic/rounded_rect.h>

namespace zaf {

class RoundedRectangleGeometry : public Geometry {
public:
    RoundedRectangleGeometry() = default;
    RoundedRectangleGeometry(ID2D1RoundedRectangleGeometry* handle) : Geometry(handle) { }

    RoundedRect GetRoundedRect() const {
        D2D1_ROUNDED_RECT rounded_rect = { 0 };
        GetHandle()->GetRoundedRect(&rounded_rect);
        return RoundedRect::FromD2D1ROUNDEDRECT(rounded_rect);
    }

    ID2D1RoundedRectangleGeometry* GetHandle() const {
        return static_cast<ID2D1RoundedRectangleGeometry*>(__super::GetHandle());
    }
};

}