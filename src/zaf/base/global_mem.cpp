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
    auto memory = GlobalMem::Alloc(memory_size, flags);
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
    //A discarded memory has zero size, which is not a failure.
    if (size == 0) {
        ZAF_THROW_IF_WIN32_ERROR(GetLastError());
    }
    return size;
}


void GlobalMem::ReAlloc(std::size_t new_size) {

    auto new_handle = GlobalReAlloc(handle_, new_size, GMEM_ZEROINIT);
    if (!new_handle) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }

    handle_ = new_handle;
}


bool GlobalMem::IsDiscarded() const {
    
    UINT flags = GlobalFlags(handle_);
    if (flags == GMEM_INVALID_HANDLE) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }
    return (flags & GMEM_DISCARDED) != 0;
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