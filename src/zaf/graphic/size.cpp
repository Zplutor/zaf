#include <zaf/graphic/size.h>
#include <zaf/parsing/parsers/size_parser.h>
#include <zaf/reflection/reflection_type_definition.h>

namespace zaf {

ZAF_DEFINE_EQUALITY_TYPE(Size)

ZAF_DEFINE_REFLECTION_TYPE(Size)
    ZAF_DEFINE_PARSER(SizeParser)
ZAF_DEFINE_END

}