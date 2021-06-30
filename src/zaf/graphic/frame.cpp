#include <zaf/graphic/frame.h>
#include <zaf/parsing/parsers/frame_parser.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_EQUALITY(Frame)

ZAF_DEFINE_TYPE(Frame)
    ZAF_DEFINE_PARSER(FrameParser)
ZAF_DEFINE_END

}