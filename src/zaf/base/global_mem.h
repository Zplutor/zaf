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


class GlobalMemLock : NonCopyable {
public:
    GlobalMemLock(HGLOBAL handle, LPVOID pointer) noexcept : handle_(handle), pointer_(pointer) {

    }

    GlobalMemLock(GlobalMemLock&& other) noexcept :
        handle_(other.handle_),
        pointer_(other.pointer_) {

        other.handle_ = nullptr;
        other.pointer_ = nullptr;
    }

    GlobalMemLock& operator=(GlobalMemLock&& other) noexcept {
        if (this != &other) {
            Reset();
            handle_ = other.handle_;
            pointer_ = other.pointer_;
            other.handle_ = nullptr;
            other.pointer_ = nullptr;
        }
        return *this;
    }

    ~GlobalMemLock() {
        Reset();
    }

    LPVOID Pointer() const noexcept {
        return pointer_;
    }

private:
    void Reset() noexcept {
        if (handle_) {
            GlobalUnlock(handle_);
            handle_ = nullptr;
            pointer_ = nullptr;
        }
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
    explicit GlobalMem(HGLOBAL handle) noexcept : handle_(handle) {

    }

    ~GlobalMem() {
        Reset();
    }

    GlobalMem(GlobalMem&& other) noexcept : handle_(other.Detach()) {

    }

    GlobalMem& operator=(GlobalMem&& other) noexcept {
        if (this != &other) {
            Reset(other.Detach());
        }
        return *this;
    }

    std::size_t Size() const;

    void Resize(std::size_t new_size);

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