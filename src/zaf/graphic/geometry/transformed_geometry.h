#pragma once

#include <zaf/graphic/geometry/geometry.h>
#include <zaf/graphic/matrix.h>

namespace zaf {

/**
 Represents a geometry that has been transformed.

 You can create instances via ResourceFactory::CreateTransformedGeometry.
 */
class TransformedGeometry : public Geometry {
public:
    /**
     Construct the instance with specified handle.

     See also Geometry::Geometry.
     */
    TransformedGeometry(ID2D1TransformedGeometry* handle) : 
        Geometry(handle),
        handle_(handle) {
    
    }

    /**
     Get the source geometry of this transformed geometry.
     */
    const std::shared_ptr<Geometry> GetSourceGeometry() const {
        ID2D1Geometry* handle = nullptr;
        handle_->GetSourceGeometry(&handle);
        return std::make_shared<Geometry>(handle);
    }

    /**
     Get the matrix used to transform the source geometry.
     */
    const TransformMatrix GetTransformMatrix() const {
        D2D1::Matrix3x2F matrix;
        handle_->GetTransform(&matrix);
        return TransformMatrix::FromD2D1MATRIX3X2F(matrix);
    }

private:
    ID2D1TransformedGeometry* handle_;
};

}