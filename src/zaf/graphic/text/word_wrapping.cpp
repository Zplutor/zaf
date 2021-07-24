#include <zaf/graphic/text/word_wrapping.h>
#include <zaf/object/enum_definition.h>

namespace zaf {

ZAF_DEFINE_ENUM(WordWrapping)
ZAF_DEFINE_ENUM_VALUE(Wrap)
ZAF_DEFINE_ENUM_VALUE(NoWrap)
ZAF_DEFINE_ENUM_VALUE(EmergencyBreak)
ZAF_DEFINE_ENUM_VALUE(WholeWord)
ZAF_DEFINE_ENUM_VALUE(Character)
ZAF_DEFINE_ENUM_END

}