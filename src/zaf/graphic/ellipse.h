#pragma once 

#include <zaf/graphic/point.h>

namespace zaf {
	
class Ellipse {
public:
	Ellipse() : x_radius(0), y_radius(0) { }

	const D2D1_ELLIPSE ToD2D1ELLIPSE() const {

		D2D1_ELLIPSE ellipse = { 0 };
		ellipse.point = position.ToD2D1POINT2F();
		ellipse.radiusX = x_radius;
		ellipse.radiusY = y_radius;
		return ellipse;
	}

public:
	Point position;
	float x_radius;
	float y_radius;
};

}