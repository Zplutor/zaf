#pragma once

#include <zaf/graphic/geometry/geometry.h>

namespace zaf {

class EllipseGeometry : public Geometry {
public:
    explicit EllipseGeometry(COMPtr<ID2D1EllipseGeometry> inner) : Geometry(std::move(inner)) { }
};

}