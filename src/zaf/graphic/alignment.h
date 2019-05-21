#pragma once

#include <zaf/graphic/ellipse.h>
#include <zaf/graphic/rounded_rect.h>

namespace zaf {

Point Align(const Point& point);
Point AlignLine(const Point& point, float stroke_width);

Rect Align(const Rect& rect);
Rect AlignLine(const Rect& rect, float stroke_width);

RoundedRect Align(const RoundedRect& rounded_rect);
RoundedRect AlignLine(const RoundedRect& rounded_rect, float stroke_width);

Ellipse Align(const Ellipse& ellipse);
Ellipse AlignLine(const Ellipse& ellipse, float stroke_width);

float AlignmentOffsetForLine(float stroke_width);

}