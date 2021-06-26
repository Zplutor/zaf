#include <zaf/graphic/point.h>
#include <zaf/parsing/parsers/point_parser.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_EQUALITY_TYPE(Point)

ZAF_DEFINE_TYPE(Point)
    ZAF_DEFINE_PARSER(PointParser)
ZAF_DEFINE_END

}