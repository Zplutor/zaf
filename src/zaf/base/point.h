#pragma once

namespace zaf {

class Point {
public:
	Point() : x(0), y(0) { }
	Point(float x, float y) : x(x), y(y) { }

	float x;
	float y;
};

}