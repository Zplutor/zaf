#pragma once

/**
@file
    Defines the `zaf::rx::Disposable` class.
*/

#include <mutex>
#include <optional>
#include <vector>
#include <zaf/base/closure.h>
#include <zaf/base/non_copyable.h>

namespace zaf::rx {

/**
Represents an object that can be disposed, such as releasing resources or cancelling operations.

@details
    This is a base class for disposable objects. Derived classes should implement the 
    `IsDisposed()` and `EnsureDisposed()` methods to manage their disposal state.

    Disposing can be considered as an explicit way to release resources or cancel ongoing
    operations rather than relying on the destructor only. This allows more flexible control over 
    the cleanup actions, especially in scenarios where the object's lifecycle is indeterminate.

    When implementing a disposable class, besides the guidelines mentioned in the `IsDisposed()` 
    and `EnsureDisposed()` methods, the following guidelines should be followed as well:
    1. The object should not be disposed when it is created.
    2. The destructor should perform the same disposal actions as what `EnsureDisposed()` does.
*/
class Disposable : NonCopyableNonMovable {
public:
    /**
    Gets an empty `Disposable` instance whose `IsDisposed()` always returns true and `Dispose()` 
    does nothing.

    @return
        The singleton empty `Disposable` instance.

    @post
        The return value is not null.

    @throw std::bad_alloc
        Thrown if the empty instance cannot be created.
    */
    static const std::shared_ptr<Disposable>& Empty();

public:
    Disposable() noexcept;
    virtual ~Disposable() = default;

    /**
    Indicates whether the object has been disposed.

    @return
        True if the object has been disposed; otherwise, false.

    @details
        This method is thread-safe. Derived classes should implement this method in a thread-safe 
        manner.
    */
    virtual bool IsDisposed() const noexcept = 0;

    /**
    Disposes the object.

    @details
        This method is idempotent and thread-safe. It can be called multiple times but will only
        perform the disposal action once.

        This method calls `EnsureDisposed()` to perform the actual disposal action.
    */
    void Dispose() noexcept;

    /**
    Adds a callback to be invoked when the object is disposed.

    @param callback
        The callback to invoke when the object is disposed. The callback should not throw
        exceptions, otherwise it may lead to undefined behavior.

    @pre
        The callback is not null.

    @throw zaf::PreconditionError
    @throw std::bad_alloc

    @details
        This method is thread-safe. If the object is not disposed when this method is called, the
        callback will be stored and invoked after the object is disposed. If the object is already
        disposed, the callback won't be stored and will be invoked immediately.

        If the object is destructed without being disposed, the stored callbacks will not be 
        invoked.

        @note
            The order of invoking the callbacks is not guaranteed.
    */
    void AddDisposedCallback(Closure callback);

protected:
    /**
    Ensures that the object is disposed, performing the actual disposal action.

    @return
        True if the disposal action was performed; otherwise, false.

    @details
        This method is called by `Dispose()`. Derived classes should implement this method in a 
        thread-safe manner. They must ensure that only one call to this method performs the
        disposal action and returns true. Subsequent calls should return false without performing 
        any action.
    */
    virtual bool EnsureDisposed() noexcept = 0;

private:
    void InvokeDisposedCallbacks() noexcept;

private:
    std::mutex disposed_callbacks_mutex_;
    // Will be set to std::nullopt after disposed.
    std::optional<std::vector<Closure>> disposed_callbacks_;
};

}