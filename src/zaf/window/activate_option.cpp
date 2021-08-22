#include <zaf/window/activate_option.h>
#include <zaf/object/enum_definition.h>

namespace zaf {

ZAF_DEFINE_ENUM(ActivateOption)
ZAF_DEFINE_ENUM_VALUE(None)
ZAF_DEFINE_ENUM_VALUE(NoActivate)
ZAF_DEFINE_ENUM_VALUE(DiscardMouseMessage)
ZAF_DEFINE_ENUM_END

}