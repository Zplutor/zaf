#pragma once

#include <zaf/base/direct2d.h>

namespace zaf {

enum class InterpolationMode {
    NearestNeighbor = D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
    Linear = D2D1_INTERPOLATION_MODE_LINEAR,
    Cubic = D2D1_INTERPOLATION_MODE_CUBIC,
    MultiSampleLinear = D2D1_INTERPOLATION_MODE_MULTI_SAMPLE_LINEAR,
    Anisotropic = D2D1_INTERPOLATION_MODE_ANISOTROPIC,
    HighQualityCubic = D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC,
};

}