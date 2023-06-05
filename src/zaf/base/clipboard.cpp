#include <zaf/base/clipboard.h>
#include <strsafe.h>
#include <zaf/base/error/com_error.h>
#include <zaf/base/error/system_error.h>
#include <zaf/base/global_mem.h>
#include <zaf/base/non_copyable.h>

namespace zaf {
namespace {

class ClipboardGuard : NonCopyableNonMovable {
public:
    ClipboardGuard() = default;

    ~ClipboardGuard() {
        CloseClipboard();
    }
};

}

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


void Clipboard::SetText(std::wstring_view text) {

    BOOL is_succeeded = OpenClipboard(nullptr);
    if (!is_succeeded) {
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }

    ClipboardGuard guard;

    is_succeeded = EmptyClipboard();
    if (!is_succeeded) {
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }

    {
        auto memory = GlobalMem::FromString(text, GlobalMemFlags::Movable);

        auto data_handle = SetClipboardData(CF_UNICODETEXT, memory.Handle());
        if (!data_handle) {
            ZAF_THROW_SYSTEM_ERROR(GetLastError());
        }

        //According to offical documentation, the memory shouldn't be freed if SetClipboardData() 
        //succeeded.
        auto handle = memory.Detach();
    }
}

}