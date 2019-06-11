#include <zaf/parsing/parsers/size_parser.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/graphic/size.h>
#include <zaf/parsing/xaml_reader.h>

namespace zaf {

void SizeParser::Parse(XamlReader& reader, ReflectionObject& reflection_object) {

    Size& size = dynamic_cast<Size&>(reflection_object);

    auto attribute_reader = reader.GetAttributeReader();
    while (attribute_reader.Read()) {

        auto name = attribute_reader.GetName();
        if (name == L"width") {
            size.width = ToNumeric<float>(attribute_reader.GetValue());
        }
        else if (name == L"height") {
            size.height = ToNumeric<float>(attribute_reader.GetValue());
        }
    }
}

}