#include <zaf/graphic/point.h>
#include <zaf/base/as.h>
#include <zaf/base/string/to_string.h>
#include <zaf/object/type_definition.h>
#include <zaf/object/parsing/internal/utility.h>

namespace zaf {
namespace {

class PointParser : public ObjectParser {
public:
    void ParseFromAttribute(
        const std::wstring& attribute_value,
        Object& object) {

        auto& point = As<Point>(object);

        bool is_succeeded = internal::ParseAttributeToDoubleFloats(
            attribute_value, 
            point.x, 
            point.y);

        if (!is_succeeded) {
            ZAF_THROW_ERRC(BasicErrc::InvalidValue);
        }
    }
};

}

ZAF_DEFINE_TYPE(Point)
ZAF_DEFINE_TYPE_PARSER(PointParser)
ZAF_DEFINE_TYPE_PROPERTY(X)
ZAF_DEFINE_TYPE_PROPERTY(Y)
ZAF_DEFINE_TYPE_END

ZAF_DEFINE_EQUALITY(Point)


std::wstring Point::ToString() const {
    return ToWideString(x) + L',' + ToWideString(y);
}

}