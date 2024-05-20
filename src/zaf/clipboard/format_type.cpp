#include <zaf/clipboard/format_type.h>
#include <zaf/base/error/win32_error.h>

namespace zaf::clipboard {

FormatType MakeSharedFormatType(const std::wstring& format_name) {

    auto format_type = RegisterClipboardFormat(format_name.c_str());
    if (format_type == 0) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }
    return static_cast<FormatType>(format_type);
}

}