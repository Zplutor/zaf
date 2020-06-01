#include <zaf/graphic/frame.h>
#include <zaf/parsing/parsers/frame_parser.h>
#include <zaf/reflection/reflection_type_definition.h>

namespace zaf {

ZAF_DEFINE_REFLECTION_TYPE(Frame)
    ZAF_DEFINE_PARSER(FrameParser)
ZAF_DEFINE_END

}