#pragma once

#include <Windows.h>
#include <zaf/base/flag_enum.h>
#include <zaf/base/non_copyable.h>

namespace zaf {

enum class GlobalMemFlags {
    Fixed = GMEM_FIXED,
    Movable = GMEM_MOVEABLE,
    ZeroInit = GMEM_ZEROINIT,
};

ZAF_ENABLE_FLAG_ENUM(GlobalMemFlags);


class GlobalMemLock : NonCopyableNonMovable {
public:
    GlobalMemLock(HGLOBAL handle, LPVOID pointer) : handle_(handle), pointer_(pointer) {

    }

    ~GlobalMemLock() {
        if (handle_) {
            GlobalUnlock(handle_);
        }
    }

    LPVOID Pointer() const {
        return pointer_;
    }

private:
    HGLOBAL handle_{};
    LPVOID pointer_{};
};


class GlobalMem : NonCopyable {
public:
    static GlobalMem Alloc(std::size_t size, GlobalMemFlags flags);
    static GlobalMem FromString(std::wstring_view string, GlobalMemFlags flags);

public:
    explicit GlobalMem(HGLOBAL handle) : handle_(handle) {

    }

    ~GlobalMem() {
        Reset();
    }

    GlobalMem(GlobalMem&& other) : handle_(other.Detach()) {

    }

    GlobalMem& operator=(GlobalMem&& other) {
        if (this != &other) {
            Reset(other.Detach());
        }
        return *this;
    }

    std::size_t Size() const;

    void Reset(HGLOBAL new_handle = nullptr) {
        if (handle_ != new_handle) {
            if (handle_) {
                GlobalFree(handle_);
            }
            handle_ = new_handle;
        }
    }

    [[nodiscard]]
    HGLOBAL Detach() {
        auto result = handle_;
        handle_ = nullptr;
        return result;
    }

    HGLOBAL Handle() const {
        return handle_;
    }

    bool IsValid() const {
        return !!handle_;
    }

    explicit operator bool() const {
        return IsValid();
    }

    [[nodiscard]]
    GlobalMemLock Lock() const;

    std::wstring ToString() const;

private:
    HGLOBAL handle_{};
};

}