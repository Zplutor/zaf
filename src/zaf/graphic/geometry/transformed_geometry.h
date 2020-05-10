#pragma once

#include <zaf/graphic/geometry/geometry.h>
#include <zaf/graphic/matrix.h>

namespace zaf {

/**
 Represents a geometry that has been transformed.

 You can create instances via GraphicFactory::CreateTransformedGeometry.
 */
class TransformedGeometry : public Geometry {
public:
    TransformedGeometry() = default;

    /**
     Construct the instance with specified handle.

     See also Geometry::Geometry.
     */
    TransformedGeometry(ID2D1TransformedGeometry* handle) : Geometry(handle) { }

    /**
     Get the source geometry of this transformed geometry.
     */
    std::shared_ptr<Geometry> GetSourceGeometry() const {
        ID2D1Geometry* handle = nullptr;
        GetActualHandle()->GetSourceGeometry(&handle);
        return std::make_shared<Geometry>(handle);
    }

    /**
     Get the matrix used to transform the source geometry.
     */
    TransformMatrix GetTransformMatrix() const {
        D2D1::Matrix3x2F matrix;
        GetActualHandle()->GetTransform(&matrix);
        return TransformMatrix::FromD2D1MATRIX3X2F(matrix);
    }

private:
    ID2D1TransformedGeometry* GetActualHandle() const {
        return static_cast<ID2D1TransformedGeometry*>(GetHandle());
    }
};

}