#pragma once

#include <zaf/base/direct2d.h>

namespace zaf {

/**
 Describes the width and height of a size.	
 */
class Size {
public:
	/**
	 Convert a specified D2D1_SIZE_F structure to Size.
	 */
	static Size FromD2D1SIZEF(const D2D1_SIZE_F& size) {
		return Size(size.width, size.height);
	}

	/**
	 Convert a specified D2D1_SIZE_U structure to Size.
	 */
	static Size FromD2D1SIZEU(const D2D1_SIZE_U& size) {

		return Size(
			static_cast<float>(size.width),
			static_cast<float>(size.height)
		);
	}

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

	/**
	 Convert to D2D1_SIZE_F structure.
	 */
	D2D1_SIZE_F ToD2D1SIZEF() const {

		D2D1_SIZE_F size;
		size.width = width;
		size.height = height;
		return size;
	}

	/**
	 Convert to D2D1_SIZE_U structure.
	 */
	D2D1_SIZE_U ToD2D1SIZEU() const {

		D2D1_SIZE_U size;
		size.width = static_cast<UINT32>(width);
		size.height = static_cast<UINT32>(height);
		return size;
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