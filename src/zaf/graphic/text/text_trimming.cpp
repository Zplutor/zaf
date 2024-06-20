#include <zaf/graphic/text/text_trimming.h>
#include <zaf/object/enum_definition.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_ENUM(TextTrimmingGranularity)
ZAF_DEFINE_ENUM_VALUE(None)
ZAF_DEFINE_ENUM_VALUE(Character)
ZAF_DEFINE_ENUM_VALUE(Word)
ZAF_DEFINE_ENUM_END


ZAF_OBJECT_IMPL(TextTrimming);

}