#include <zaf/graphic/font/font_weight.h>

namespace zaf {

ZAF_OBJECT_IMPL(FontWeight);
ZAF_DEFINE_EQUALITY(FontWeight)

std::wstring FontWeight::ToString() const {
    return std::to_wstring(value_);
}

}