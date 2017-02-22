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

const Point MakeClearEdgePointForLine(const Point& point, float stroke_width, ClearEdgeOption option);
const Point MakeClearEdgePointForFill(const Point& point, ClearEdgeOption option);

const Rect MakeClearEdgeRectForLine(const Rect& rect, float stroke_width, ClearEdgeOption option);
const Rect MakeClearEdgeRectForFill(const Rect& rect, ClearEdgeOption option);

const RoundedRect MakeClearEdgeRoundedRectForLine(const RoundedRect& rounded_rect, float stroke_width, ClearEdgeOption option);
const RoundedRect MakeClearEdgeRoundedRectForFill(const RoundedRect& rounded_rect, ClearEdgeOption option);

const Ellipse MakeClearEdgeEllipseForLine(const Ellipse& ellipse, float stroke_width, ClearEdgeOption option);
const Ellipse MakeClearEdgeEllipseForFill(const Ellipse& ellipse, ClearEdgeOption option);

}