#pragma once

#include <zaf/graphic/ellipse.h>
#include <zaf/graphic/point.h>
#include <zaf/graphic/rect.h>
#include <zaf/graphic/rounded_rect.h>

namespace zaf {

enum class ClearEdgeOption {
	None,
	Clear,
};

Point MakeClearEdgeForLine(const Point& point, float stroke_width, ClearEdgeOption option);
Point MakeClearEdgeForFill(const Point& point, ClearEdgeOption option);

Rect MakeClearEdgeForLine(const Rect& rect, float stroke_width, ClearEdgeOption option);
Rect MakeClearEdgeForFill(const Rect& rect, ClearEdgeOption option);

RoundedRect MakeClearEdgeForLine(const RoundedRect& rounded_rect, float stroke_width, ClearEdgeOption option);
RoundedRect MakeClearEdgeForFill(const RoundedRect& rounded_rect, ClearEdgeOption option);

Ellipse MakeClearEdgeForLine(const Ellipse& ellipse, float stroke_width, ClearEdgeOption option);
Ellipse MakeClearEdgeForFill(const Ellipse& ellipse, ClearEdgeOption option);

}