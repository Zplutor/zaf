#include <zaf/clipboard/format.h>

namespace zaf::clipboard {
namespace {

bool AreFORMATETCsEqual(const FORMATETC& format1, const FORMATETC& format2) {
    return
        (format1.cfFormat == format2.cfFormat) &&
        (format1.dwAspect == format2.dwAspect) &&
        (format1.lindex == format2.lindex) &&
        (format1.ptd == format2.ptd) &&
        (format1.tymed == format2.tymed);
}

}


Format::Format() {

    inner_.dwAspect = DVASPECT_CONTENT;
    inner_.lindex = -1;
}


Format::Format(FormatType format_type, clipboard::MediumType medium_type) : Format() {

    inner_.cfFormat = static_cast<CLIPFORMAT>(format_type);
    inner_.tymed = static_cast<DWORD>(medium_type);
}


Format::Format(FormatType format_type) : Format(format_type, clipboard::MediumType::GlobalMem) {

}


Format::Format(const FORMATETC& inner) : inner_(inner) {

}


bool Format::operator==(const Format& other) const {
    return AreFORMATETCsEqual(inner_, other.Inner());
}


bool Format::operator==(const FORMATETC& other) const {
    return AreFORMATETCsEqual(inner_, other);
}

}