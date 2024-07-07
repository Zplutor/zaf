#include <zaf/graphic/font/font_weight.h>
#include <zaf/base/as.h>

namespace zaf {

ZAF_OBJECT_IMPL(FontWeight);
ZAF_DEFINE_EQUALITY(FontWeight)

std::wstring FontWeight::ToString() const {
    return std::to_wstring(value_);
}


void FontWeight::CloneFrom(const Object& other) {
    *this = As<FontWeight>(other);
}

}