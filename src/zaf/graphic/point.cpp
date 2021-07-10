#include <zaf/graphic/point.h>
#include <zaf/object/type_definition.h>
#include <zaf/parsing/parsers/internal/utility.h>

namespace zaf {
namespace {

class PointParser : public ObjectParser {
public:
    void ParseFromAttribute(
        const std::wstring& attribute_value,
        Object& object) {

        auto& point = dynamic_cast<Point&>(object);
        internal::ParseAttributeToDoubleFloats(attribute_value, point.x, point.y);
    }
};

}

ZAF_DEFINE_TYPE(Point)
ZAF_DEFINE_TYPE_PARSER(PointParser)
ZAF_DEFINE_PROPERTY_WITH_FIELD(X, x)
ZAF_DEFINE_PROPERTY_WITH_FIELD(Y, y)
ZAF_DEFINE_TYPE_END

ZAF_DEFINE_EQUALITY(Point)

}