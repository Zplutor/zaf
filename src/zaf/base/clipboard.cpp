#include <zaf/base/clipboard.h>
#include <strsafe.h>
#include <zaf/base/error/com_error.h>
#include <zaf/base/error/system_error.h>
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


class GlobalMemoryGuard : NonCopyableNonMovable {
public:
    GlobalMemoryGuard(HGLOBAL handle) : handle_(handle) { };

    ~GlobalMemoryGuard() {
        if (handle_) {
            GlobalFree(handle_);
        }
    }

    HGLOBAL Handle() const {
        return handle_;
    }

    void Detach() {
        handle_ = nullptr;
    }

private:
    HGLOBAL handle_{};
};


class GlobalLockGuard : NonCopyableNonMovable {
public:
    GlobalLockGuard(HGLOBAL handle) : handle_(handle) { }

    ~GlobalLockGuard() {
        if (handle_) {
            GlobalUnlock(handle_);
        }
    }

private:
    HGLOBAL handle_{};
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
        auto memory_size = (text.length() + 1) * sizeof(wchar_t);
        GlobalMemoryGuard memory_guard = GlobalAlloc(GMEM_MOVEABLE, memory_size);
        if (!memory_guard.Handle()) {
            ZAF_THROW_SYSTEM_ERROR(GetLastError());
        }

        LPVOID memory = GlobalLock(memory_guard.Handle());
        if (!memory) {
            ZAF_THROW_SYSTEM_ERROR(GetLastError());
        }

        {
            GlobalLockGuard lock_guard(memory_guard.Handle());

            HRESULT hresult = StringCchCopy(
                reinterpret_cast<wchar_t*>(memory),
                text.length() + 1,
                text.data());

            ZAF_THROW_IF_COM_ERROR(hresult);
        }

        auto data_handle = SetClipboardData(CF_UNICODETEXT, memory_guard.Handle());
        if (!data_handle) {
            ZAF_THROW_SYSTEM_ERROR(GetLastError());
        }

        //According to offical documentation, the memory shouldn't be freed if SetClipboardData() 
        //succeeded.
        memory_guard.Detach();
    }
}

}