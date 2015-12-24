#pragma once

#include <zaf/base/point.h>
#include <zaf/base/size.h>

namespace zaf {

class Rect {
public:
	static Rect Intersect(const Rect& rect1, const Rect& rect2);
	static Rect Union(const Rect& rect1, const Rect& rect2);

public:
	Rect() { }
	Rect(const Point& position, const Size& size) : position(position), size(size) { }
	Rect(const Size& size) : position(), size(size) { }
	Rect(float x, float y, float width, float height) : position(x, y), size(width, height) { }

	Rect& operator=(const Rect& other) {
		position = other.position;
		size = other.size;
		return *this;
	}

	bool HasIntersection(const Rect& other) const;
	void Intersect(const Rect& other);
	void Union(const Rect& other);
	void Inflate(float x, float y);
	bool Contain(const Point& point) const;

	bool IsEmpty() const {

		return 
			(position.x == 0) && 
			(position.y == 0) && 
			(size.width == 0) && 
			(size.height == 0);
	}

	Point position;
	Size size;
};

}