#pragma once

namespace zaf {

class Ellipse;
class Point;
class Rect;

enum class ClearEdgeOption {
	None,
	Clear,
};

const Point MakeClearEdgePointForLine(const Point& point, float stroke_width, ClearEdgeOption option);
const Point MakeClearEdgePointForFill(const Point& point, ClearEdgeOption option);

const Rect MakeClearEdgeRectForLine(const Rect& rect, float stroke_width, ClearEdgeOption option);
const Rect MakeClearEdgeRectForFill(const Rect& rect, ClearEdgeOption option);

const Ellipse MakeClearEdgeEllipseForLine(const Ellipse& ellipse, float stroke_width, ClearEdgeOption option);
const Ellipse MakeClearEdgeEllipseForFill(const Ellipse& ellipse, ClearEdgeOption option);

}