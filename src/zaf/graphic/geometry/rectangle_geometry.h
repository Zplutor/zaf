#pragma once

#include <zaf/graphic/rect.h>
#include <zaf/graphic/geometry/geometry.h>

namespace zaf {

class RectangleGeometry : public Geometry {
public:
    explicit RectangleGeometry(ID2D1RectangleGeometry* handle) : 
        Geometry(handle),
        handle_(handle) {
    
    }

    const Rect GetRect() const {

        D2D1_RECT_F rect = { 0 };
        handle_->GetRect(&rect);
        return Rect::FromD2D1RECTF(rect);
    }

private:
    ID2D1RectangleGeometry* handle_;
};

}