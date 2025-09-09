#include <zaf/graphic/point_parser.h>
#include <zaf/graphic/point.h>
#include <zaf/dynamic/parsing/parse_error.h>
#include <zaf/dynamic/parsing/internal/utility.h>

namespace zaf {

void PointParser::ParseFromAttribute(
    const std::wstring& attribute_value,
    Object& object) {

    auto& point = As<Point>(object);

    bool is_succeeded = internal::ParseAttributeToDoubleFloats(
        attribute_value,
        point.x,
        point.y);

    if (!is_succeeded) {
        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }
}

}