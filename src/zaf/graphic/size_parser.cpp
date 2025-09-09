#include <zaf/graphic/size_parser.h>
#include <zaf/base/as.h>
#include <zaf/graphic/size.h>
#include <zaf/dynamic/parsing/internal/utility.h>
#include <zaf/dynamic/parsing/parse_error.h>

namespace zaf {

void SizeParser::ParseFromAttribute(const std::wstring& attribute_value, Object& object) {

    auto& size = As<Size>(object);

    bool is_succeeded = internal::ParseAttributeToDoubleFloats(
        attribute_value,
        size.width,
        size.height);

    if (!is_succeeded) {
        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }
}

}