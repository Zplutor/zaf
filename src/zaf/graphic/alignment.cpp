#include <zaf/graphic/alignment.h>

namespace zaf {
namespace {

Point AlignPoint(const Point& point, float offset) {
    float x = std::round(point.x) + offset;
    float y = std::round(point.y) + offset;
    return Point(x, y);
}


Rect AlignRect(const Rect& rect, float offset) {
    float left = std::round(rect.position.x) + offset;
    float top = std::round(rect.position.y) + offset;
    float right = std::round(rect.position.x + rect.size.width) - offset;
    float bottom = std::round(rect.position.y + rect.size.height) - offset;
    return Rect(left, top, right - left, bottom - top);
}


RoundedRect AlignRoundedRect(const RoundedRect& rounded_rect, float offset) {
    auto rect = AlignRect(rounded_rect.rect, offset);
    float x_radius = std::round(rounded_rect.x_radius) + offset;
    float y_radius = std::round(rounded_rect.y_radius) + offset;
    return RoundedRect(rect, x_radius, y_radius);
}


Ellipse AlignEllipse(const Ellipse& ellipse, float offset) {

    float left = std::round(ellipse.position.x - ellipse.x_radius) + offset;
    float top = std::round(ellipse.position.y - ellipse.y_radius) + offset;
    float right = std::round(ellipse.position.x + ellipse.x_radius) - offset;
    float bottom = std::round(ellipse.position.y + ellipse.y_radius) - offset;

    float x_radius = right - left;
    if (x_radius != 0) {
        x_radius /= 2;
    }

    float y_radius = bottom - top;
    if (y_radius != 0) {
        y_radius /= 2;
    }

    return Ellipse(left + x_radius, top + y_radius, x_radius, y_radius);
}

}


Point Align(const Point& point) {
    return AlignPoint(point, 0);
}

Point AlignLine(const Point& point, float stroke_width) {
    return AlignPoint(point, AlignmentOffsetForLine(stroke_width));
}


Rect Align(const Rect& rect) {
    return AlignRect(rect, 0);
}

Rect AlignLine(const Rect& rect, float stroke_width) {
    return AlignRect(rect, AlignmentOffsetForLine(stroke_width));
}


RoundedRect Align(const RoundedRect& rounded_rect) {
    return AlignRoundedRect(rounded_rect, 0);
}

RoundedRect AlignLine(const RoundedRect& rounded_rect, float stroke_width) {
    return AlignRoundedRect(rounded_rect, AlignmentOffsetForLine(stroke_width));
}


Ellipse Align(const Ellipse& ellipse) {
    return AlignEllipse(ellipse, 0);
}

Ellipse AlignLine(const Ellipse& ellipse, float stroke_width) {
    return AlignEllipse(ellipse, AlignmentOffsetForLine(stroke_width));
}


float AlignmentOffsetForLine(float stroke_width) {
    if (static_cast<int>(std::ceil(stroke_width)) % 2 != 0) {
        return 0.5;
    }
    else {
        return 0;
    }
}

}