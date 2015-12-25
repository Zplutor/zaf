#pragma once

namespace zaf {

/**
 Describes the width and height of a size.	
 */
class Size {
public:
	/**
	 Initialize the instance that has zero width and height.
	 */
	Size() : width(0), height(0) { }

	/**
	 Initialize the instance that is the same as another Size. 
	 */
	Size(const Size& other) : width(other.width), height(other.height) { }

	/**
	 Initialize the instance that has specified width and height. 
	 */
	Size(float width, float height) : width(width), height(height) { }

	/**
	 Assign from another Size instance.
	 */
	Size& operator=(const Size& other) {

		width = other.width;
		height = other.height;
		return *this;
	}

public:
	/**
	 Width of the size.
	 */
	float width;

	/**
	 Height of the size.
	 */
	float height;
};

}