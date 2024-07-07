#include <zaf/graphic/size.h>
#include <zaf/base/string/to_string.h>

namespace zaf {

ZAF_OBJECT_IMPL(Size);
ZAF_DEFINE_EQUALITY(Size)


std::wstring Size::ToString() const {
    return ToWideString(width) + L',' + ToWideString(height);
}


void Size::CloneFrom(const Object& other) {
    *this = As<Size>(other);
}

}