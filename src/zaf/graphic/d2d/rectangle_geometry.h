#pragma once

#include <zaf/graphic/rect.h>
#include <zaf/graphic/d2d/geometry.h>

namespace zaf::d2d {

/**
 Represents a rectangle.   

 You can create instances via GraphicFactory::CreateRectangleGeometry.
 */
class RectangleGeometry : public Geometry {
public:
    RectangleGeometry() = default;
    explicit RectangleGeometry(COMPtr<ID2D1RectangleGeometry> inner) :
        Geometry(inner),
        inner_(std::move(inner)) { }

    /**
     Get the rectangle area.
     */
    Rect GetRect() const {

        D2D1_RECT_F rect = { 0 };
        Inner()->GetRect(&rect);
        return Rect::FromD2D1RECTF(rect);
    }

    const COMPtr<ID2D1RectangleGeometry>& Inner() const noexcept {
        return inner_;
    }

private:
    COMPtr<ID2D1RectangleGeometry> inner_;
};

}