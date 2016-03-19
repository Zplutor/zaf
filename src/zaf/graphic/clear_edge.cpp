#include <zaf/graphic/clear_edge.h>
#include <cmath>
#include <functional>
#include <utility>
#include <zaf/base/assert.h>
#include <zaf/base/rect.h>

namespace zaf {

typedef float(AdjustCoordinateFunctionPrototype)(float);
typedef std::function<AdjustCoordinateFunctionPrototype> AdjustCoordinateFunction;

static float CalculateOffsetForLine(float strokw_width);
static const Rect MakeClearEdgeRect(const Rect& rect, float offset, ClearEdgeOption option);
static std::pair<AdjustCoordinateFunction, AdjustCoordinateFunction> GetAdjustCoordinateFunctions(ClearEdgeOption option);


const Point MakeClearEdgePointForLine(const Point& point, float stroke_width, ClearEdgeOption option) {

	if (option == ClearEdgeOption::None) {
		return point;
	}

	float offset = CalculateOffsetForLine(stroke_width);

	float x = std::round(point.x) + offset;
	float y = std::round(point.y) + offset;
	return Point(x, y);
}


const Rect MakeClearEdgeRectForLine(const Rect& rect, float stroke_width, ClearEdgeOption option) {

	if (option == ClearEdgeOption::None) {
		return rect;
	}

	float offset = CalculateOffsetForLine(stroke_width);

	float left = std::round(rect.position.x) + offset;
	float top = std::round(rect.position.y) + offset;
	float right = std::round(rect.position.x + rect.size.width) - offset;
	float bottom = std::round(rect.position.y + rect.size.height) - offset;
	return Rect(left, top, right - left, bottom - top);
}


const Rect MakeClearEdgeRectForFill(const Rect& rect, ClearEdgeOption option) {

	if (option == ClearEdgeOption::None) {
		return rect;
	}

	float left = std::round(rect.position.x);
	float top = std::round(rect.position.y);
	float right = std::round(rect.position.x + rect.size.width);
	float bottom = std::round(rect.position.y + rect.size.height);
	return Rect(left, top, right - left, bottom - top);
}


static float CalculateOffsetForLine(float strokw_width) {

	if (static_cast<int>(std::ceil(strokw_width)) % 2 != 0) {
		return 0.5;
	}
	else {
		return 0;
	}
}


}