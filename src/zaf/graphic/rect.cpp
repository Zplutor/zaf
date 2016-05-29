#include <zaf/graphic/rect.h>
#include <algorithm>

#undef max
#undef min

namespace zaf {

void Rect::Intersect(const Rect& other) {

	float left = std::max(position.x, other.position.x);
	float top = std::max(position.y, other.position.y);
	float right = std::min(position.x + size.width, other.position.x + other.size.width);
	float bottom = std::min(position.y + size.height, other.position.y + other.size.height);

	if ((left < right) && (top < bottom)) {
		position.x = left;
		position.y = top;
		size.width = right - left;
		size.height = bottom - top;
	}
	else {
		position.x = 0;
		position.y = 0;
		size.width = 0;
		size.height = 0;
	}
}

}