#include <zaf/graphic/interpolation_mode.h>
#include <zaf/object/enum_definition.h>

namespace zaf {

ZAF_DEFINE_ENUM(InterpolationMode)
ZAF_DEFINE_ENUM_VALUE(NearestNeighbor)
ZAF_DEFINE_ENUM_VALUE(Linear)
ZAF_DEFINE_ENUM_VALUE(Cubic)
ZAF_DEFINE_ENUM_VALUE(MultiSampleLinear)
ZAF_DEFINE_ENUM_VALUE(Anisotropic)
ZAF_DEFINE_ENUM_VALUE(HighQualityCubic)
ZAF_DEFINE_ENUM_END

}
