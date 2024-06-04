#include <zaf/base/global_mem.h>
#include <strsafe.h>
#include <zaf/base/error/win32_error.h>

namespace zaf {

GlobalMem GlobalMem::Alloc(std::size_t size, GlobalMemFlags flags) {

    auto handle = GlobalAlloc(static_cast<UINT>(flags), size);
    if (!handle) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }
    return GlobalMem{ handle };
}


GlobalMem GlobalMem::FromString(std::wstring_view string, GlobalMemFlags flags) {

    auto length_with_null = string.length() + 1;

    std::size_t memory_size = length_with_null * sizeof(wchar_t);
    auto memory = GlobalMem::Alloc(memory_size, GlobalMemFlags::Movable);
    {
        auto lock = memory.Lock();
        StringCchCopy(
            reinterpret_cast<wchar_t*>(lock.Pointer()),
            length_with_null,
            string.data());
    }
    return memory;
}


std::size_t GlobalMem::Size() const {

    auto size = GlobalSize(handle_);
    if (size == 0) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }
    return size;
}


GlobalMemLock GlobalMem::Lock() const {

    LPVOID pointer = GlobalLock(handle_);
    if (!pointer) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }
    return GlobalMemLock{ handle_, pointer };
}


std::wstring GlobalMem::ToString() const {

    auto lock = Lock();
    return std::wstring{ reinterpret_cast<const wchar_t*>(lock.Pointer()) };
}

}