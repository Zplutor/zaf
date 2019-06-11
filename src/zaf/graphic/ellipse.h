#pragma once 

#include <zaf/graphic/point.h>

namespace zaf {
	
class Ellipse : public ReflectionObject {
public:
    ZAF_DECLARE_REFLECTION_TYPE();

public:
	Ellipse() : x_radius(0), y_radius(0) { }

	Ellipse(float x, float y, float x_radius, float y_radius) :
		position(x, y),
		x_radius(x_radius),
		y_radius(y_radius) {
	
	}

	Ellipse(const Point& position, float x_radius, float y_radius) : 
		position(position),
		x_radius(x_radius),
		y_radius(y_radius) {

	}

	void Inflate(float radius) {

		x_radius += radius;
		y_radius += radius;
	}

	void Inflate(float x_radius, float y_radius) {

		this->x_radius += x_radius;
		this->y_radius += y_radius;
	}

	D2D1_ELLIPSE ToD2D1ELLIPSE() const {

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