#include <zaf/base/range.h>
#include <zaf/base/string/to_string.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(Range)
ZAF_DEFINE_TYPE_PROPERTY(Index)
ZAF_DEFINE_TYPE_PROPERTY(Length)
ZAF_DEFINE_TYPE_END

ZAF_DEFINE_EQUALITY(Range);

std::wstring Range::ToString() const {
    return ToWideString(Index()) + L',' + ToWideString(Length());
}

}