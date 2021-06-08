#pragma once

#include <zaf/graphic/geometry/geometry.h>

namespace zaf {

class EllipseGeometry : public Geometry {
public:
	explicit EllipseGeometry(ID2D1EllipseGeometry* handle) : Geometry(handle) { }
};

}