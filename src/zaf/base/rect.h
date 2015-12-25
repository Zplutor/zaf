#pragma once

#include <Windows.h>
#include <zaf/base/point.h>
#include <zaf/base/size.h>

namespace zaf {

/**
 Describes the position and size of a rectangle area.
 */
class Rect {
public:
	/**
	 Convert a specified RECT structure to Rect.
	 */
	static Rect FromRECT(const RECT& rect) {

		return Rect(
			static_cast<float>(rect.left),
			static_cast<float>(rect.top),
			static_cast<float>(rect.right - rect.left),
			static_cast<float>(rect.bottom - rect.top)
		);
	}

	/**
	 Create a Rect instance that has the intersection rectangle of other two instances.
	 */
	static Rect Intersect(const Rect& rect1, const Rect& rect2) {
		Rect result = rect1;
		result.Intersect(rect2);
		return result;
	}

	/**
	 Create a Rect instance that has the union rectangle of other two instances.
	 */
	static Rect Union(const Rect& rect1, const Rect& rect2) {
		Rect result = rect1;
		result.Union(rect2);
		return result;
	}

public:
	/**
	 Initialize the instance that has an empty rectangle.
	 */
	Rect() { }

	/**
	 Initialize the instance that is the same as another Rect.
	 */
	Rect(const Rect& other) : position(other.position), size(other.size) { }

	/**
	 Initialize the instance that has specified position and size.
	 */
	Rect(const Point& position, const Size& size) : position(position), size(size) { }

	/**
	 Initialize the instance that has zero positon and specified size.
	 */
	explicit Rect(const Size& size) : position(), size(size) { }

	/**
	 Initialize the instance that has specified x-coordinate, y-coordinate, width and height.
  	 */
	Rect(float x, float y, float width, float height) : position(x, y), size(width, height) { }

	/**
	 Assign from another Rect instance.
	 */
	Rect& operator=(const Rect& other) {
		position = other.position;
		size = other.size;
		return *this;
	}

	/**
	 Determine whether the rectangle has intersection with another Rect.
	 */
	bool HasIntersection(const Rect& other) const {
		Rect result = Intersect(*this, other);
		return !result.IsEmpty();
	}

	/**
	 Make an intersection rectangle with another Rect.
	 */
	void Intersect(const Rect& other) {

		RECT rect1 = this->ToRECT();
		RECT rect2 = other.ToRECT();
		RECT result = { 0 };
		IntersectRect(&result, &rect1, &rect2);
		*this = Rect::FromRECT(result);
	}

	/**
	 Make an union rectangle with another Rect.
	 */
	void Union(const Rect& other) {

		RECT rect1 = this->ToRECT();
		RECT rect2 = other.ToRECT();
		RECT result = { 0 };
		UnionRect(&result, &rect1, &rect2);
		*this = Rect::FromRECT(result);
	}

	/**
	 Modify the size of the rectangle in-place.
	 */
	void Inflate(float width, float height) {
		position.x -= width;
		position.y -= height;
		size.width += width * 2;
		size.height += height * 2;
	}

	/**
	 Determine whether the specified point locates within the rectangle.
	 */
	bool Contain(const Point& point) const {

		return 
			(point.x >= position.x) && 
			(point.x <= position.x + size.width) &&
			(point.y >= position.y) &&
			(point.y <= position.y + size.height);
	}

	/**
	 Determine whether the ractangle is empty.
	 */
	bool IsEmpty() const {

		return 
			(position.x == 0) && 
			(position.y == 0) && 
			(size.width == 0) && 
			(size.height == 0);
	}

	/**
	 Convert to RECT structure.
	 */
	RECT ToRECT() const {
		RECT rect;
		rect.left = static_cast<LONG>(position.x);
		rect.top = static_cast<LONG>(position.y);
		rect.right = static_cast<LONG>(position.x + size.width);
		rect.bottom = static_cast<LONG>(position.y + size.height);
		return rect;
	}

public:
	/**
	 Position of the rectangle.
	 */
	Point position;

	/**
	 Size of the rectangle.
	 */
	Size size;
};

}