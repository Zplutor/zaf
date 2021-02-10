#pragma once

#include <zaf/graphic/ellipse.h>
#include <zaf/graphic/rounded_rect.h>

namespace zaf {

Point Align(const Point& point, float stroke_width = 0);
Rect Align(const Rect& rect, float stroke_width = 0);
RoundedRect Align(const RoundedRect& rounded_rect, float stroke_width = 0);
Ellipse Align(const Ellipse& ellipse, float stroke_width = 0);

float AlignmentOffsetForLine(float stroke_width);

}