#pragma once

/**
@file
    Defines the `zaf::TrayIcon` class.
*/

#include <Windows.h>
#include <zaf/base/non_copyable.h>
#include <zaf/base/unique_hicon.h>
#include <zaf/rx/disposable_host.h>
#include <zaf/window/message_only_window.h>

namespace zaf {

/**
Represents an icon in the system notification area.
*/
class TrayIcon : rx::DisposableHost, NonCopyableNonMovable {
public:
    /**
    Constructs an instance of `TrayIcon` with the specified ID.
    */
    explicit TrayIcon(const GUID& id) noexcept;
    virtual ~TrayIcon();

    /**
    Gets the ID of the icon.
    */
    const GUID& ID() const noexcept;

    /**
    Gets the icon handle for the icon.
    */
    const UniqueHICON& Icon() const noexcept;

    /**
    Sets the icon handle for the icon.

    @param icon
        The icon handle to be set.

    @throw zaf::UnknownRuntimeError
        Thrown if failed to modify the icon if derived class doesn't override `OnModifyFailed()`.

    @throw ...
        Any exception will be thrown by `OnModifyFailed()` if derived class overrides it.

    @details
        If this method is called after a successful call of `Add()`, it will try to modify the icon 
        in the system notification area. If the icon is failed to be modified, `OnModifyFailed()` 
        will be called.
    */
    void SetIcon(UniqueHICON icon);

    /**
    Gets the tooltip for the icon.
    */
    const std::wstring& Tooltip() const noexcept;

    /**
    Sets the tooltip for the icon.

    @param tooltip
        The tooltip to be set. It will be truncated if its length exceeds the maximum length 
        defined by the system when it is set to the icon.

    @throw zaf::UnknownRuntimeError
        Thrown if failed to modify the icon if derived class doesn't override `OnModifyFailed()`.

    @throw ...
        Any exception will be thrown by `OnModifyFailed()` if derived class overrides it.
        
    @details
        If this method is called after a successful call of `Add()`, it will try to modify the icon 
        in the system notification area. If the icon is failed to be modified, `OnModifyFailed()` 
        will be called.
    */
    void SetTooltip(std::wstring tooltip);

    /**
    Adds the icon to the system notification area.

    @throw zaf::InvalidOperationError
        Thrown if calling this method directly in `OnAddFailed()`, which causes reentrant.

    @throw zaf::Win32Error
        Thrown if failed to create the message window for the icon.

    @throw zaf::UnknownRuntimeError
        Thrown if failed to add the icon if derived class doesn't override `OnAddFailed()`.

    @throw ...
        Any exception will be thrown by `OnAddFailed()` if derived class overrides it.

    @details
        If the icon is failed to be added, `OnAddFailed()` will be called.
    */
    void Add();

    /**
    Removes the icon from the system notification area.

    @return
        `true` if the icon was successfully removed, `false` otherwise.
    */
    bool Remove() noexcept;

protected:
    /**
    Called when the icon is failed to be added.

    @details
        The default implementation of this method simply throws `zaf::UnknownRuntimeError`. Derived
        classes can override this method to handle the failure, such as retry after a period.
    */
    virtual void OnAddFailed();

    /**
    Called when the icon is failed to be modified.

    @details
        The default implementation of this method simply throws `zaf::UnknownRuntimeError`. Derived
        classes can override this method to handle the failure, such as retry after a period. A
        common retry method is remove the icon and add it again.
    */
    virtual void OnModifyFailed();

private:
    NOTIFYICONDATA MakeBasicIconData() const noexcept;

private:
    GUID id_{};
    std::wstring tooltip_;
    UniqueHICON icon_;
    std::optional<MessageOnlyWindow> message_window_;
    bool is_adding_{};
    bool has_added_{};
};

}