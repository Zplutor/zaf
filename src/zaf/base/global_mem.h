#pragma once

/**
@file
    Defines the class zaf::GlobalMem.
*/

#include <Windows.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/base/flag_enum.h>
#include <zaf/base/non_copyable.h>

namespace zaf {

/**
GlobalMem flags control how the memory will be allocated.
*/
enum class GlobalMemFlags {

    /**
    Allocates fixed memory.
    */
    Fixed = GMEM_FIXED,

    /**
    Allocates movable memory.
    */
    Movable = GMEM_MOVEABLE,

    /**
    Initializes memory contents to zero.
    */
    ZeroInit = GMEM_ZEROINIT,
};

ZAF_ENABLE_FLAG_ENUM(GlobalMemFlags);


/**
Represents a lock to a GlobalMem instance, providing access to the memory pointer of the GlobalMem.
*/
class GlobalMemLock : NonCopyable {
public:
    /**
    Constructs an instance with the specified HGLOBAL and the locked memory pointer.

    @param handle
        The HGLOBAL handle.

    @param pointer
        The memory pointer returned by GlobalLock().
    */
    GlobalMemLock(HGLOBAL handle, LPVOID pointer) noexcept :
        handle_(handle),
        pointer_(pointer) {

    }

    /**
    Constructs an instance by moving from another instance.
    */
    GlobalMemLock(GlobalMemLock&& other) noexcept :
        handle_(other.handle_),
        pointer_(other.pointer_) {

        other.handle_ = nullptr;
        other.pointer_ = nullptr;
    }

    /**
    Assigns the instance by moving from another instance.
    */
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

    /**
    Destructs the instance, unlocks the memory pointer.
    */
    ~GlobalMemLock() {
        Reset();
    }

    /**
    Gets the locked memory pointer.

    @return
        The locked memory pointer.
    */
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


/**
An encapsulation class for the HGLOBAL.
*/
class GlobalMem : NonCopyable {
public:
    /**
    Creates a GlobalMem instance by allocating memory of a specified size.

    @param size
        The size of the memory to allocate, in bytes. It can be zero.

    @param flags
        The flags that control how the memory is allocated.

    @return
        The new GlobalMem instance. The size of instance will be larger than the requested size.

    @throw zaf::Win32Error
        Thrown if the memory allocation fails.

    @details
        If the size is zero and the flags include GlobalMemFlags::Movable, the returned GlobalMem 
        is a discarded instance. A discarded GlobalMem has zero size, cannot be locked, and should 
        be re-allocated to a new size for further use.

        This method calls GlobalAlloc() to create the HGLOBAL.
    */
    static GlobalMem Alloc(std::size_t size, GlobalMemFlags flags);

    /**
    Creates a GlobalMem instance by copying the content of a specified string.

    @param string
        The string which will be copied to the new GlobalMem instance.

    @param flags
        The flags that control how the memory is allocated.

    @return
        The new GlobalMem instance that contains the specified string. The string is 
        null-terminated.

    @throw zaf::Win32Error
        Thrown if the memory allocation fails.

    @details
        This method calls GlobalAlloc() to create the HGLOBAL, and then copy the string into it.
    */
    static GlobalMem FromString(std::wstring_view string, GlobalMemFlags flags);

public:
    /**
    Constructs a null instance.
    */
    GlobalMem() noexcept = default;

    /**
    Constructs an instance with the specified HGLOBAL.

    @param handle
        The HGLOBAL that will be managed by the instance.

    @pre 
        handle is not null.

    @throw zaf::PreconditionError
        Thrown if the precondition is violated.

    @details
        After constructing the instance, the ownership of the HGLOBAL is transferred to the 
        instance, users should not free the HGLOBAL manually.
    */
    explicit GlobalMem(HGLOBAL handle) : handle_(handle) {
        ZAF_EXPECT(handle_);
    }

    /**
    Destructs the instance by freeing the managed HGLOBAL.
    */
    ~GlobalMem() {
        Reset();
    }

    /**
    Constructs an instance by moving from another instance.
    */
    GlobalMem(GlobalMem&& other) noexcept : handle_(other.Detach()) {

    }

    /**
    Assigns the current instance by moving from another instance.
    */
    GlobalMem& operator=(GlobalMem&& other) noexcept {
        if (this != &other) {
            Reset(other.Detach());
        }
        return *this;
    }

    /**
    Gets the size of the GlobalMem.

    @return
        The actual size of the GlobalMem. It may be larger than the requested allocation size. The 
        size of a discarded instance is zero.

    @throw zaf::Win32Error
        Thrown if fails to get the size.
    */
    std::size_t Size() const;

    /**
    Re-allocates the GlobalMem to a new size.

    @param new_size
        The new size to re-allocate.

    @throw zaf::Win32Error
        Thrown if fails to re-allocate.
    */
    void ReAlloc(std::size_t new_size);

    /**
    Resets the managed HGLOBAL to a new one.

    @param new_handle
        The new HGLOBAL to set.

    @details
        The original HGLOBAL will be freed.
    */
    void Reset(HGLOBAL new_handle = nullptr) noexcept {
        if (handle_ != new_handle) {
            if (handle_) {
                GlobalFree(handle_);
            }
            handle_ = new_handle;
        }
    }

    /**
    Detaches the managed HGLOBAL and sets the current GlobalMem to null.

    @return
        The managed HGLOBAL. The ownership of the HGLOBAL is transferred to the user, users should 
        free the HGLOBAL manually.
    */
    [[nodiscard]]
    HGLOBAL Detach() noexcept {
        auto result = handle_;
        handle_ = nullptr;
        return result;
    }

    /**
    Gets the managed HGLOBAL.

    @return
        The managed HGLOBAL.
    */
    HGLOBAL Handle() const noexcept {
        return handle_;
    }

    /**
    Indicates whether the GlobalMem is null.

    @return
        True if the managed HGLOBAL is null; otherwise false.
    */
    bool IsNull() const noexcept {
        return !!handle_;
    }

    /**
    The bool conversion operator that indicates whether the GlobalMem is not null.

    @return
        True if the managed HGLOBAL is not null; otherwise, false.
    */
    explicit operator bool() const noexcept {
        return IsNull();
    }
    
    /**
    Indicates whether the GlobalMem is discarded.

    @return 
        True if the GlobalMem is discarded; otherwise false.

    @throw
        Thrown if fails to check.
    */
    bool IsDiscarded() const;

    /**
    Locks the GlobalMem to gain access to the memory pointer.

    @return 
        The GlobalMemLock instance. Call its Pointer() method to obtain the pointer to the memory.

    @throw zaf::Win32Error
        Thrown if fails to lock. A discard GlobalMem cannot be locked.
    */
    [[nodiscard]]
    GlobalMemLock Lock() const;

    /**
    Converts the content in the GlobalMem to a string.

    @return
        The string that is converted from the GlobalMem.

    @throw zaf::Win32Error
        Thrown if fails to lock the GlobalMem.

    @note
        This method interprets the memory pointer as a null-terminated string, users must ensure 
        the content is valid before calling this method.
    */
    std::wstring ToString() const;

private:
    HGLOBAL handle_{};
};

}