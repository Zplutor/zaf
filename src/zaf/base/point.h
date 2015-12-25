#pragma once

#include <Windows.h>

namespace zaf {

/**
 Describes the x-coordinate and y-coordinate of a point. 
 */
class Point {
public:
	/**
	 Convert a specified POINT structure to Point.
	 */
	static Point FromPOINT(const POINT& point) {

		return Point(
			static_cast<float>(point.x),
			static_cast<float>(point.y)
		);
	}

public:
	/**
	 Initialize the instance that has zero x-coordinate and y-coordinate.
	 */
	Point() : x(0), y(0) { }

	/**
	 Initialize the instance that is the same as another Point.
	 */
	Point(const Point& other) : x(other.x), y(other.y) { }

	/**
	 Initialize the instance that has specified x-coordinate and y-coordinate.
	 */
	Point(float x, float y) : x(x), y(y) { }

	/**
	 Assign from another Point instance.
	 */
	Point& operator=(const Point& other) {

		x = other.x;
		y = other.y;
		return *this;
	}

	/**
	 Convert to POINT structure.
	 */
	POINT ToPOINT() const {

		POINT point;
		point.x = static_cast<LONG>(x);
		point.y = static_cast<LONG>(y);
		return point;
	}

public:
	/**
	 X-coordinate of the point.
	 */
	float x;

	/**
	 Y-coordinate of the point.
	 */
	float y;
};

}