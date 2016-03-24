#include <zaf/graphic/clear_edge.h>
#include <cmath>
#include <zaf/graphic/ellipse.h>
#include <zaf/graphic/rect.h>

namespace zaf {

static float CalculateOffsetForLine(float strokw_width);
static const Rect MakeClearEdgeRect(const Rect& rect, float offset);
static const Ellipse MakeClearEdgeEllipse(const Ellipse& ellipse, float offset);

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

	return MakeClearEdgeRect(rect, CalculateOffsetForLine(stroke_width));
}


const Rect MakeClearEdgeRectForFill(const Rect& rect, ClearEdgeOption option) {

	if (option == ClearEdgeOption::None) {
		return rect;
	}

	return MakeClearEdgeRect(rect, 0);
}


const Ellipse MakeClearEdgeEllipseForLine(const Ellipse& ellipse, float stroke_width, ClearEdgeOption option) {

	if (option == ClearEdgeOption::None) {
		return ellipse;
	}

	return MakeClearEdgeEllipse(ellipse, CalculateOffsetForLine(stroke_width));
}


const Ellipse MakeClearEdgeEllipseForFill(const Ellipse& ellipse, ClearEdgeOption option) {

	if (option == ClearEdgeOption::None) {
		return ellipse;
	}

	return MakeClearEdgeEllipse(ellipse, 0);
}


static float CalculateOffsetForLine(float strokw_width) {

	if (static_cast<int>(std::ceil(strokw_width)) % 2 != 0) {
		return 0.5;
	}
	else {
		return 0;
	}
}


static const Rect MakeClearEdgeRect(const Rect& rect, float offset) {

	float left = std::round(rect.position.x) + offset;
	float top = std::round(rect.position.y) + offset;
	float right = std::round(rect.position.x + rect.size.width) - offset;
	float bottom = std::round(rect.position.y + rect.size.height) - offset;
	return Rect(left, top, right - left, bottom - top);
}


static const Ellipse MakeClearEdgeEllipse(const Ellipse& ellipse, float offset) {

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