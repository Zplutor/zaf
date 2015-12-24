#include <zaf/base/rect.h>
#include <Windows.h>
#include <zaf/internal/conversion.h>

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

	RECT rect1 = this->ToRECT();
	RECT rect2 = other.ToRECT();
	RECT result = { 0 };
	IntersectRect(&result, &rect1, &rect2);
	*this = Rect::FromRECT(result);
}


void Rect::Union(const Rect& other) {

	RECT rect1 = this->ToRECT();
	RECT rect2 = other.ToRECT();
	RECT result = { 0 };
	UnionRect(&result, &rect1, &rect2);
	*this = Rect::FromRECT(result);
}


}