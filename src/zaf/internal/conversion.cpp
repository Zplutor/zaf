#include <zaf/internal/conversion.h>
#include <zaf/base/rect.h>

namespace zaf {

D2D1_RECT_F ToD2dRect(const Rect& rect) {

	D2D1_RECT_F d2d_rect;
	d2d_rect.left = rect.position.x;
	d2d_rect.top = rect.position.y;
	d2d_rect.right = rect.position.x + rect.size.width;
	d2d_rect.bottom = rect.position.y + rect.size.height;

	return d2d_rect;
}


}