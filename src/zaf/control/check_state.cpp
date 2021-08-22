#include <zaf/control/check_state.h>
#include <zaf/object/enum_definition.h>

namespace zaf {

ZAF_DEFINE_ENUM(CheckState)
ZAF_DEFINE_ENUM_VALUE(Unchecked)
ZAF_DEFINE_ENUM_VALUE(Checked)
ZAF_DEFINE_ENUM_VALUE(Indeterminate)
ZAF_DEFINE_ENUM_END

}