#pragma once

#include <zaf/graphic/rect.h>

namespace zaf {

inline float ToDIPs(float pixels, float dpi) {
    return pixels / (dpi / 96.f);
}

inline float FromDIPs(float dips, float dpi) {
    return dips * (dpi / 96.f);
}


inline Point ToDIPs(const Point& point, float dpi) {
    return Point{
        ToDIPs(point.x, dpi),
        ToDIPs(point.y, dpi)
    };
}

inline Point FromDIPs(const Point& point, float dpi) {
    return Point{
        FromDIPs(point.x, dpi),
        FromDIPs(point.y, dpi)
    };
}


inline Size ToDIPs(const Size& size, float dpi) {
    return Size{
        ToDIPs(size.width, dpi),
        ToDIPs(size.height, dpi)
    };
}

inline Size FromDIPs(const Size& size, float dpi) {
    return Size{
        FromDIPs(size.width, dpi),
        FromDIPs(size.height, dpi)
    };
}


inline Rect ToDIPs(const Rect& rect, float dpi) {
    return Rect{
        ToDIPs(rect.position, dpi),
        ToDIPs(rect.size, dpi)
    };
}

inline Rect FromDIPs(const Rect& rect, float dpi) {
    return Rect{
        FromDIPs(rect.position, dpi),
        FromDIPs(rect.size, dpi)
    };
}

}