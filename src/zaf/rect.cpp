#include "rect.h"
#include <Windows.h>
#include "internal/conversion.h"

namespace zaf {

Rect Rect::Intersect(const Rect& rect1, const Rect& rect2) {

	Rect result = rect1;
	result.Intersect(rect2);
	return result;
}


Rect Rect::Union(const Rect& rect1, const Rect& rect2) {

	Rect result = rect1;
	result.Union(rect2);
	return result;
}


bool Rect::HasIntersection(const Rect& other) const {

	Rect result = Intersect(*this, other);
	return ! result.IsEmpty();
}


void Rect::Intersect(const Rect& other) {

	RECT rect1 = ToWin32Rect(*this);
	RECT rect2 = ToWin32Rect(other);
	RECT result = { 0 };
	IntersectRect(&result, &rect1, &rect2);
	*this = FromWin32Rect(result);
}


void Rect::Union(const Rect& other) {

	RECT rect1 = ToWin32Rect(*this);
	RECT rect2 = ToWin32Rect(other);
	RECT result = { 0 };
	UnionRect(&result, &rect1, &rect2);
	*this = FromWin32Rect(result);
}


void Rect::Inflate(float x, float y) {

	position.x -= x;
	position.y -= y;
	size.width += x * 2;
	size.height += y * 2;
}


bool Rect::Contain(const Point& point) const {

	if ( (point.x < position.x) ||
		 (point.x > position.x + size.width) ) {
		return false;
	}

	if ( (point.y < position.y) || 
		 (point.y > position.y + size.height) ) {
		return false;
	}

	return true;
}


}