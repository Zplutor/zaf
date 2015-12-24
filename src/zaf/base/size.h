#pragma once

namespace zaf {

class Size {
public:
	Size() : width(0), height(0) { }
	Size(float width, float height) : width(width), height(height) { }

	float width;
	float height;
};

}