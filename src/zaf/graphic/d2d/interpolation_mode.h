#pragma once

#include <zaf/base/direct2d.h>
#include <zaf/object/enum_support.h>

namespace zaf::d2d {

enum class InterpolationMode {
    NearestNeighbor = D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
    Linear = D2D1_INTERPOLATION_MODE_LINEAR,
    Cubic = D2D1_INTERPOLATION_MODE_CUBIC,
    MultiSampleLinear = D2D1_INTERPOLATION_MODE_MULTI_SAMPLE_LINEAR,
    Anisotropic = D2D1_INTERPOLATION_MODE_ANISOTROPIC,
    HighQualityCubic = D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC,
};

ZAF_ENUM_BEGIN(InterpolationMode)
ZAF_ENUM_CONSTANT(NearestNeighbor)
ZAF_ENUM_CONSTANT(Linear)
ZAF_ENUM_CONSTANT(Cubic)
ZAF_ENUM_CONSTANT(MultiSampleLinear)
ZAF_ENUM_CONSTANT(Anisotropic)
ZAF_ENUM_CONSTANT(HighQualityCubic)
ZAF_ENUM_END;

}