#include <zaf/clipboard/clipboard.h>
#include <strsafe.h>
#include <zaf/base/error/com_error.h>
#include <zaf/base/error/win32_error.h>
#include <zaf/base/global_mem.h>
#include <zaf/base/non_copyable.h>

namespace zaf::clipboard {
namespace {

class ClipboardGuard : NonCopyableNonMovable {
public:
    ClipboardGuard() {

        BOOL is_succeeded = OpenClipboard(nullptr);
        if (!is_succeeded) {
            ZAF_THROW_WIN32_ERROR(GetLastError());
        }
    }

    ~ClipboardGuard() {
        CloseClipboard();
    }
};

}

std::wstring Clipboard::GetText() {

    ClipboardGuard clipboard;

    HANDLE handle = GetClipboardData(CF_UNICODETEXT);
    if (!handle) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }

    auto pointer = GlobalLock(handle);
    if (!pointer) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }

    GlobalMemLock lock{ handle, pointer };
    return std::wstring{ reinterpret_cast<wchar_t*>(pointer) };
}


void Clipboard::SetText(std::wstring_view text) {

    ClipboardGuard guard;

    BOOL is_succeeded = EmptyClipboard();
    if (!is_succeeded) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }

    {
        auto memory = GlobalMem::FromString(text, GlobalMemFlags::Movable);

        auto data_handle = SetClipboardData(CF_UNICODETEXT, memory.Handle());
        if (!data_handle) {
            ZAF_THROW_WIN32_ERROR(GetLastError());
        }

        //According to offical documentation, the memory shouldn't be freed if SetClipboardData() 
        //succeeded.
        auto handle = memory.Detach();
    }
}


DataObject Clipboard::GetDataObject() {

    COMPtr<IDataObject> ptr;
    HRESULT hresult = OleGetClipboard(ptr.Reset());
    ZAF_THROW_IF_COM_ERROR(hresult);

    return DataObject{ ptr };
}


void Clipboard::SetDataObject(const DataObject& data_object) {

    HRESULT hresult = OleSetClipboard(data_object.Ptr().Inner());
    ZAF_THROW_IF_COM_ERROR(hresult);
}

}