#pragma once

#include <zaf/graphic/d2d/geometry.h>
#include <zaf/graphic/matrix.h>

namespace zaf::d2d {

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
    TransformedGeometry(COMPtr<ID2D1TransformedGeometry> inner) :
        Geometry(inner),
        inner_(std::move(inner)) { }

    /**
     Get the source geometry of this transformed geometry.
     */
    std::shared_ptr<Geometry> GetSourceGeometry() const {
        COMPtr<ID2D1Geometry> inner;
        Inner()->GetSourceGeometry(inner.Reset());
        return std::make_shared<Geometry>(inner);
    }

    /**
     Get the matrix used to transform the source geometry.
     */
    TransformMatrix GetTransformMatrix() const {
        D2D1::Matrix3x2F matrix;
        Inner()->GetTransform(&matrix);
        return TransformMatrix::FromD2D1MATRIX3X2F(matrix);
    }

    const COMPtr<ID2D1TransformedGeometry>& Inner() const noexcept {
        return inner_;
    }

private:
    COMPtr<ID2D1TransformedGeometry> inner_;
};

}