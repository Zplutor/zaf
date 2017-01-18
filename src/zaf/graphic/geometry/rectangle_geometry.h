#pragma once

#include <zaf/graphic/rect.h>
#include <zaf/graphic/geometry/geometry.h>

namespace zaf {

/**
 Represents a rectangle.   

 You can create instances via ResourceFactory::CreateRectangleGeometry.
 */
class RectangleGeometry : public Geometry {
public:
    RectangleGeometry() { }

    /**
     Construct the instance with specified handle.

     See also Geometry::Geometry.
     */
    explicit RectangleGeometry(ID2D1RectangleGeometry* handle) : Geometry(handle) { }

    /**
     Get the rectangle area.
     */
    const Rect GetRect() const {

        D2D1_RECT_F rect = { 0 };
        GetActualHandle()->GetRect(&rect);
        return Rect::FromD2D1RECTF(rect);
    }

private:
    ID2D1RectangleGeometry* GetActualHandle() const {
        return dynamic_cast<ID2D1RectangleGeometry*>(GetHandle());
    }
};

}