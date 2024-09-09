#pragma once

#include <zaf/graphic/d2d/geometry.h>
#include <zaf/graphic/matrix.h>
#include <zaf/graphic/rect.h>

namespace zaf::d2d {

class LayerParameters {
public:
    LayerParameters() {

        SetContentBounds(Rect::Infinite);
        SetMaskTransform(TransformMatrix::Identity);
        SetOpacity(1);
    }

    void SetContentBounds(const Rect& value) {
        inner_.contentBounds = value.ToD2D1RECTF();
    }
    
    void SetGeometricMask(const Geometry& value) {
        geometric_mask_ = value;
        inner_.geometricMask = geometric_mask_.Ptr().Inner();
    }

    void SetMaskTransform(const TransformMatrix& value) {
        inner_.maskTransform = value.ToD2D1MATRIX3X2F();
    }

    float Opacity() const {
        return inner_.opacity;
    }

    void SetOpacity(float value) {
        inner_.opacity = value;
    }

    const D2D1_LAYER_PARAMETERS& Inner() const {
        return inner_;
    }

private:
    D2D1_LAYER_PARAMETERS inner_{};
    Geometry geometric_mask_;
};

}