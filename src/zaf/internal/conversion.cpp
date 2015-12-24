#include <zaf/internal/conversion.h>
#include <zaf/base/point.h>
#include <zaf/base/rect.h>

namespace zaf {

RECT ToWin32Rect(const Rect& rect) {

	RECT win32_rect;
	win32_rect.left = rect.position.x;
	win32_rect.top = rect.position.y;
	win32_rect.right = rect.position.x + rect.size.width;
	win32_rect.bottom = rect.position.y + rect.size.height;

	return win32_rect;
}


Rect FromWin32Rect(const RECT& win32_rect) {

	return Rect(
		win32_rect.left, 
		win32_rect.top, 
		win32_rect.right - win32_rect.left,
		win32_rect.bottom - win32_rect.top
	);
}


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