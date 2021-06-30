#pragma once

#include <Windows.h>
#include <memory>
#include <zaf/base/direct2d.h>
#include <zaf/base/hash.h>
#include <zaf/base/relation_operator.h>
#include <zaf/object/equality.h>
#include <zaf/object/object.h>

namespace zaf {

/**
 Describes the x-coordinate and y-coordinate of a point. 
 */
class Point : public Object {
public:
	ZAF_DECLARE_TYPE
	ZAF_DECLARE_EQUALITY

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

	/**
     Convert a specified D2D1_POINT_2F structure to Point.
	 */
	static Point FromD2D1POINT2F(const D2D1_POINT_2F& point) {
		return Point(point.x, point.y);
	}

    /**
     Convert a specified D2D1_POINT_2U structure to Point.
     */
    static Point FromD2D1POINT2U(const D2D1_POINT_2U& point) {
        return Point(static_cast<float>(point.x), static_cast<float>(point.y));
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

	void AddOffset(const Point& offset) {
		x += offset.x;
		y += offset.y;
	}

	void SubtractOffset(const Point& offset) {
		x -= offset.x;
		y -= offset.y;
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

	/**
	 Convert to D2D1_POINT_2F structure.
	 */
	D2D1_POINT_2F ToD2D1POINT2F() const {

		D2D1_POINT_2F point;
		point.x = x;
		point.y = y;
		return point;
	}

    /**
     Convert to D2D1_POINT_2U structuer.
     */
    D2D1_POINT_2U ToD2D1POINT2U() const {

        D2D1_POINT_2U point;
        point.x = static_cast<UINT32>(x);
        point.y = static_cast<UINT32>(y);
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


inline bool operator==(const Point& point1, const Point& point2) {
    return (point1.x == point2.x) && (point1.y == point2.y);
}


inline bool operator<(const Point& point1, const Point& point2) {

    if (point1.x < point2.x) {
        return true;
    }

    if (point1.x > point2.x) {
        return false;
    }

    return point1.y < point2.y;
}


ZAF_DEFINE_RELATION_OPERATORS(Point);

}


namespace std {
template<>
struct hash<zaf::Point> {
	std::size_t operator()(const zaf::Point& point) {
		return zaf::CalculateHash(point.x, point.y);
	}
};
}