#include <zaf/base/clipboard.h>

namespace zaf {

std::wstring Clipboard::GetText() {

    BOOL is_succeeded = OpenClipboard(nullptr);
    if (!is_succeeded) {
        return {};
    }

    std::wstring result;

    HANDLE data = GetClipboardData(CF_UNICODETEXT);
    if (data) {
        result.assign(reinterpret_cast<wchar_t*>(data));
    }

    CloseClipboard();
    return result;
}

}