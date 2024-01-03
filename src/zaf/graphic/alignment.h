#pragma once

#include <zaf/graphic/ellipse.h>
#include <zaf/graphic/rounded_rect.h>

namespace zaf {

//Point
Point ToPixelAlignedInPixels(const Point& point, float stroke_width, float dpi);
inline Point ToPixelAlignedInPixels(const Point& point, float dpi) {
    return ToPixelAlignedInPixels(point, 0, dpi);
}

Point ToPixelAligned(const Point& point, float stroke_width, float dpi);
inline Point ToPixelAligned(const Point& point, float dpi) {
    return ToPixelAligned(point, 0, dpi);
}


//Rect
Rect ToPixelAlignedInPixels(const Rect& rect, float stroke_width, float dpi);
inline Rect ToPixelAlignedInPixels(const Rect& rect, float dpi) {
    return ToPixelAlignedInPixels(rect, 0, dpi);
}

Rect ToPixelAligned(const Rect& rect, float stroke_width, float dpi);
inline Rect ToPixelAligned(const Rect& rect, float dpi) {
    return ToPixelAligned(rect, 0, dpi);
}


//RoundedRect
RoundedRect ToPixelAligned(const RoundedRect& rounded_rect, float stroke_width, float dpi);
inline RoundedRect ToPixelAligned(const RoundedRect& rounded_rect, float dpi) {
    return ToPixelAligned(rounded_rect, 0, dpi);
}


//Ellipse
Ellipse ToPixelAligned(const Ellipse& ellipse, float stroke_width, float dpi);
inline Ellipse ToPixelAligned(const Ellipse& ellipse, float dpi) {
    return ToPixelAligned(ellipse, 0, dpi);
}


float AlignmentOffsetForLine(float stroke_width, float dpi);

}