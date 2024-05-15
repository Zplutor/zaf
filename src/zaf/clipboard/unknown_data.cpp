#include <zaf/clipboard/unknown_data.h>
#include <zaf/base/error/invalid_operation_error.h>

namespace zaf::clipboard {

clipboard::Medium UnknownData::SaveToMedium(const clipboard::Format& format) {

    if (format.Type() == format_type_) {
        return medium_;
    }

    throw InvalidOperationError{ ZAF_SOURCE_LOCATION() };
}


void UnknownData::LoadFromMedium(
    clipboard::FormatType format_type,
    const clipboard::Medium& medium) {

    format_type_ = format_type;
    medium_ = medium;
}

}