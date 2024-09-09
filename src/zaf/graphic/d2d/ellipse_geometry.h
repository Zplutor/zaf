#pragma once

#include <zaf/graphic/d2d/geometry.h>

namespace zaf::d2d {

class EllipseGeometry : public Geometry {
public:
    explicit EllipseGeometry(COMPtr<ID2D1EllipseGeometry> inner) : Geometry(std::move(inner)) { }
};

}