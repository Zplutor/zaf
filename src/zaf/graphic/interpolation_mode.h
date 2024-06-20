#pragma once

#include <zaf/base/direct2d.h>
#include <zaf/object/enum_declaration.h>
#include <zaf/object/enum_support.h>

namespace zaf {

enum class InterpolationMode {
    NearestNeighbor = D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
    Linear = D2D1_INTERPOLATION_MODE_LINEAR,
    Cubic = D2D1_INTERPOLATION_MODE_CUBIC,
    MultiSampleLinear = D2D1_INTERPOLATION_MODE_MULTI_SAMPLE_LINEAR,
    Anisotropic = D2D1_INTERPOLATION_MODE_ANISOTROPIC,
    HighQualityCubic = D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC,
};

ZAF_ENUM_BEGIN(InterpolationMode)
ZAF_ENUM_VALUE(NearestNeighbor)
ZAF_ENUM_VALUE(Linear)
ZAF_ENUM_VALUE(Cubic)
ZAF_ENUM_VALUE(MultiSampleLinear)
ZAF_ENUM_VALUE(Anisotropic)
ZAF_ENUM_VALUE(HighQualityCubic)
ZAF_ENUM_END;

ZAF_ENABLE_ENUM_BOXING(InterpolationMode)

}