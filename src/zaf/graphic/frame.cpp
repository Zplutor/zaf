#include <zaf/graphic/frame.h>
#include <zaf/base/string/to_string.h>

namespace zaf {

ZAF_OBJECT_IMPL(Frame)
ZAF_DEFINE_EQUALITY(Frame)


std::wstring Frame::ToString() const {
    return
        ToWideString(left) + L',' +
        ToWideString(top) + L',' +
        ToWideString(right) + L',' +
        ToWideString(bottom);
}

}