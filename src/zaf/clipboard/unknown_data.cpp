#include <zaf/clipboard/unknown_data.h>
#include <zaf/base/error/basic_error.h>

namespace zaf::clipboard {

clipboard::Medium UnknownData::SaveToMedium(const clipboard::Format& format) {

    if (format == format_) {
        return medium_;
    }

    ZAF_THROW_ERRC(BasicErrc::Unsupported);
}


void UnknownData::LoadFromMedium(
    const clipboard::Format& format,
    const clipboard::Medium& medium) {

    format_ = format;
    medium_ = medium;
}

}