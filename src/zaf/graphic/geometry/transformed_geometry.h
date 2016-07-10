#pragma once

#include <zaf/graphic/geometry/geometry.h>
#include <zaf/graphic/matrix.h>

namespace zaf {

class TransformedGeometry : public Geometry {
public:
    TransformedGeometry(ID2D1TransformedGeometry* handle) : 
        Geometry(handle),
        handle_(handle) {
    
    }

    const std::shared_ptr<Geometry> GetSourceGeometry() const {
        ID2D1Geometry* handle = nullptr;
        handle_->GetSourceGeometry(&handle);
        return std::make_shared<Geometry>(handle);
    }

    const TransformMatrix GetTransformMatrix() const {
        D2D1::Matrix3x2F matrix;
        handle_->GetTransform(&matrix);
        return TransformMatrix::FromD2D1MATRIX3X2F(matrix);
    }

private:
    ID2D1TransformedGeometry* handle_;
};

}