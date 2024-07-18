#pragma once

#include <zaf/base/direct2d.h>
#include <zaf/base/hash.h>
#include <zaf/graphic/rect.h>

namespace zaf {

class RoundedRect : public Object {
public:
    ZAF_OBJECT;
    ZAF_DECLARE_EQUALITY;

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

    void AddOffset(const Point& offset) {
        rect.AddOffset(offset);
    }

    void SubtractOffset(const Point& offset) {
        rect.SubtractOffset(offset);
    }

    D2D1_ROUNDED_RECT ToD2D1ROUNDEDRECT() const {
        D2D1_ROUNDED_RECT d2d1_rounded_rect;
        d2d1_rounded_rect.rect = rect.ToD2D1RECTF();
        d2d1_rounded_rect.radiusX = x_radius;
        d2d1_rounded_rect.radiusY = y_radius;
        return d2d1_rounded_rect;
    }

    std::wstring ToString() const override;

    const Rect& Rect() const {
        return rect;
    }

    void SetRect(const zaf::Rect& rect) {
        this->rect = rect;
    }

    float XRadius() const {
        return x_radius;
    }

    void SetXRadius(float x_radius) {
        this->x_radius = x_radius;
    }

    float YRadius() const {
        return y_radius;
    }

    void SetYRadius(float y_radius) {
        this->y_radius = y_radius;
    }

public:
    zaf::Rect rect;
    float x_radius;
    float y_radius;
};

ZAF_OBJECT_BEGIN(RoundedRect);
ZAF_OBJECT_PROPERTY(XRadius);
ZAF_OBJECT_PROPERTY(YRadius);
ZAF_OBJECT_PROPERTY(Rect);
ZAF_OBJECT_END;


bool operator==(const RoundedRect& rounded_rect1, const RoundedRect& rounded_rect2);
bool operator<(const RoundedRect& rounded_rect1, const RoundedRect& rounded_rect2);

ZAF_DEFINE_RELATION_OPERATORS(RoundedRect);

}


template<>
struct std::hash<zaf::RoundedRect> {
    std::size_t operator()(const zaf::RoundedRect& rounded_rect) {
        return zaf::CalculateHash(rounded_rect.rect, rounded_rect.x_radius, rounded_rect.y_radius);
    }
};