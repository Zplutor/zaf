#include <zaf/clipboard/clipboard.h>
#include <strsafe.h>
#include <zaf/base/error/com_error.h>
#include <zaf/base/error/system_error.h>
#include <zaf/base/global_mem.h>
#include <zaf/base/non_copyable.h>

namespace zaf::clipboard {
namespace {

class ClipboardGuard : NonCopyableNonMovable {
public:
    ClipboardGuard() {

        BOOL is_succeeded = OpenClipboard(nullptr);
        if (!is_succeeded) {
            ZAF_THROW_SYSTEM_ERROR(GetLastError());
        }
    }

    ~ClipboardGuard() {
        CloseClipboard();
    }
};

}

std::wstring Clipboard::GetText() {

    ClipboardGuard clipbaord;

    HANDLE data = GetClipboardData(CF_UNICODETEXT);
    if (!data) {
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }

    return std::wstring{ reinterpret_cast<wchar_t*>(data) };
}


void Clipboard::SetText(std::wstring_view text) {

    ClipboardGuard guard;

    BOOL is_succeeded = EmptyClipboard();
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