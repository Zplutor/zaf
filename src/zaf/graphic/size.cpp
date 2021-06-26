#include <zaf/graphic/size.h>
#include <zaf/parsing/parsers/size_parser.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_EQUALITY_TYPE(Size)

ZAF_DEFINE_TYPE(Size)
    ZAF_DEFINE_PARSER(SizeParser)
ZAF_DEFINE_END

}