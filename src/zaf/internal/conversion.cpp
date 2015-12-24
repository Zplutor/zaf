#include <zaf/internal/conversion.h>
#include <zaf/base/point.h>
#include <zaf/base/rect.h>

namespace zaf {

POINT ToWin32Point(const Point& point) {

	POINT win32_point;
	win32_point.x = point.x;
	win32_point.y = point.y;
	return win32_point;
}


Point FromWin32Point(const POINT& win32_point) {

	return Point(
		static_cast<float>(win32_point.x),
		static_cast<float>(win32_point.y)
	);
}


D2D1_RECT_F ToD2dRect(const Rect& rect) {

	D2D1_RECT_F d2d_rect;
	d2d_rect.left = rect.position.x;
	d2d_rect.top = rect.position.y;
	d2d_rect.right = rect.position.x + rect.size.width;
	d2d_rect.bottom = rect.position.y + rect.size.height;

	return d2d_rect;
}


}