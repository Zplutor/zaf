#include <zaf/clipboard/data_descriptor.h>

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


DataDescriptor DataDescriptor::FromFormatType(clipboard::FormatType format_type) noexcept {
    return FromFormatType(format_type, clipboard::MediumType::GlobalMem);
}


DataDescriptor DataDescriptor::FromFormatType(
    clipboard::FormatType format_type,
    clipboard::MediumType medium_types) noexcept {

    DataDescriptor result;
    result.SetFormatType(format_type);
    result.SetMediumTypes(medium_types);
    return result;
}


DataDescriptor::DataDescriptor() noexcept {

    inner_.dwAspect = DVASPECT_CONTENT;
    inner_.lindex = -1;
}


DataDescriptor::DataDescriptor(const FORMATETC& inner) noexcept : inner_(inner) {

}


bool DataDescriptor::operator==(const DataDescriptor& other) const {
    return AreFORMATETCsEqual(inner_, other.Inner());
}


bool DataDescriptor::operator==(const FORMATETC& other) const {
    return AreFORMATETCsEqual(inner_, other);
}

}