#include <zaf/control/image_layout.h>
#include <zaf/object/enum_definition.h>

namespace zaf {

ZAF_DEFINE_ENUM(ImageLayout)
ZAF_DEFINE_ENUM_VALUE(None)
ZAF_DEFINE_ENUM_VALUE(Tile)
ZAF_DEFINE_ENUM_VALUE(Center)
ZAF_DEFINE_ENUM_VALUE(Stretch)
ZAF_DEFINE_ENUM_VALUE(Zoom)
ZAF_DEFINE_ENUM_END

}