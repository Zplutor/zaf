#pragma once

#include <zaf/graphic/geometry/geometry.h>
#include <zaf/graphic/rounded_rect.h>

namespace zaf {

class RoundedRectangleGeometry : public Geometry {
public:
    RoundedRectangleGeometry() = default;
    RoundedRectangleGeometry(COMPtr<ID2D1RoundedRectangleGeometry> inner) : 
        Geometry(inner),
        inner_(std::move(inner)) { }

    RoundedRect GetRoundedRect() const {
        D2D1_ROUNDED_RECT rounded_rect = { 0 };
        Inner()->GetRoundedRect(&rounded_rect);
        return RoundedRect::FromD2D1ROUNDEDRECT(rounded_rect);
    }

    const COMPtr<ID2D1RoundedRectangleGeometry>& Inner() const noexcept {
        return inner_;
    }

private:
    COMPtr<ID2D1RoundedRectangleGeometry> inner_;
};

}