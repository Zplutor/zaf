#pragma once

#include <zaf/base/direct2d.h>
#include <zaf/graphic/rect.h>

namespace zaf {

class RoundedRect : public ReflectionObject {
public:
    ZAF_DECLARE_REFLECTION_TYPE();

public:
    static const RoundedRect FromD2D1ROUNDEDRECT(const D2D1_ROUNDED_RECT& d2d1_rounded_rect) {
        return RoundedRect(
            Rect::FromD2D1RECTF(d2d1_rounded_rect.rect),
            d2d1_rounded_rect.radiusX,
            d2d1_rounded_rect.radiusY);
    }

public:
    RoundedRect() : x_radius(0), y_radius(0) { }

    RoundedRect(const Rect& rect, float x_radius, float y_radius) : 
        rect(rect),
        x_radius(x_radius), 
        y_radius(y_radius) {
    
    }

    D2D1_ROUNDED_RECT ToD2D1ROUNDEDRECT() const {
        D2D1_ROUNDED_RECT d2d1_rounded_rect;
        d2d1_rounded_rect.rect = rect.ToD2D1RECTF();
        d2d1_rounded_rect.radiusX = x_radius;
        d2d1_rounded_rect.radiusY = y_radius;
        return d2d1_rounded_rect;
    }

public:
    Rect rect;
    float x_radius;
    float y_radius;
};

}