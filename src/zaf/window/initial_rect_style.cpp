#include <zaf/window/initial_rect_style.h>
#include <zaf/object/enum_definition.h>

namespace zaf {

ZAF_DEFINE_ENUM(InitialRectStyle)
ZAF_DEFINE_ENUM_VALUE(CenterInScreen)
ZAF_DEFINE_ENUM_VALUE(CenterInOwner)
ZAF_DEFINE_ENUM_VALUE(Custom)
ZAF_DEFINE_ENUM_END

}