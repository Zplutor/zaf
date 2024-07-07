#include <zaf/graphic/point.h>
#include <zaf/base/string/to_string.h>

namespace zaf {

ZAF_OBJECT_IMPL(Point);
ZAF_DEFINE_EQUALITY(Point)


std::wstring Point::ToString() const {
    return ToWideString(x) + L',' + ToWideString(y);
}


void Point::CloneFrom(const Object& other) {
    *this = As<Point>(other);
}

}