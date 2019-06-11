#include <zaf/parsing/parsers/point_parser.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/graphic/point.h>
#include <zaf/parsing/xaml_reader.h>

namespace zaf {

void PointParser::Parse(XamlReader& reader, ReflectionObject& reflection_object) {

    Point& point = dynamic_cast<Point&>(reflection_object);

    auto attribute_reader = reader.GetAttributeReader();
    while (attribute_reader.Read()) {

        auto name = attribute_reader.GetName();
        if (name == L"x") {
            point.x = ToNumeric<float>(attribute_reader.GetValue());
        }
        else if (name == L"y") {
            point.y = ToNumeric<float>(attribute_reader.GetValue());
        }
    }
}

}