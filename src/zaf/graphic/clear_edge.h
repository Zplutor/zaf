#pragma once

namespace zaf {

class Point;
class Rect;

enum class ClearEdgeOption {
	None,
	Clear,
};

const Point MakeClearEdgePointForLine(const Point& point, float stroke_width, ClearEdgeOption option);
const Rect MakeClearEdgeRectForLine(const Rect& rect, float stroke_width, ClearEdgeOption option);
const Rect MakeClearEdgeRectForFill(const Rect& rect, ClearEdgeOption option);

}