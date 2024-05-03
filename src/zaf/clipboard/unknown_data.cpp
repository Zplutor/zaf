#include <zaf/clipboard/unknown_data.h>
#include <zaf/base/error/not_supported_error.h>

namespace zaf::clipboard {

clipboard::Medium UnknownData::SaveToMedium(const clipboard::Format& format) {

    if (format == format_) {
        return medium_;
    }

    throw NotSupportedError{ ZAF_SOURCE_SITE() };
}


void UnknownData::LoadFromMedium(
    const clipboard::Format& format,
    const clipboard::Medium& medium) {

    format_ = format;
    medium_ = medium;
}

}