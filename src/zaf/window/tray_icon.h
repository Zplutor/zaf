#pragma once

/**
@file
    Defines the `zaf::TrayIcon` class.
*/

#include <Windows.h>
#include <zaf/base/auto_reset.h>
#include <zaf/base/non_copyable.h>
#include <zaf/base/unique_hicon.h>
#include <zaf/object/object.h>
#include <zaf/rx/disposable_host.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/subject/subject.h>
#include <zaf/window/message_only_window.h>
#include <zaf/window/tray_icon_events.h>

namespace zaf {

/**
Represents an icon in the system notification area.

@details
    The `TrayIcon` class encapsulates `Shell_NotifyIcon` function to provide an easy way to manage
    icons in the system notification area.

    Since the system notification area is managed by the shell process, and `Shell_NotifyIcon` is
    actually an IPC call to the shell process, there are some special cases that may cause the call
    to fail, such as the shell process too busy to respond. In these cases, methods like `Add()`,
    `SetIcon()` and `SetTooltip()` will throw exceptions to report the failure by default. Derived
    classes can override `OnAddFailed()` and `OnModifyFailed()` to handle the failure, such as 
    retry after a period.

    The shell process may also crash or restart, which will cause the icon in the system 
    notification area to be removed. In this case, the `TrayIcon` instance will not be aware of the
    change. Users of `TrayIcon` can subscribe to the 
    `zaf::Application::SystemTaskbarCreatedEvent()` to be aware of the change, and call `Add()` to
    add the icon again. This can also be done in derived classes.

    Overall, the `TrayIcon` class provides the basic abilities to manage icons in the system 
    notification area, and users of this class should be aware of the special cases and handle them
    properly in derived classes.
*/
class TrayIcon : 
    public Object, 
    public std::enable_shared_from_this<TrayIcon>, 
    public rx::DisposableHost, 
    NonCopyableNonMovable {

public:
    ZAF_OBJECT;

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

    @throw zaf::InvalidOperationError
        Thrown if calling this method directly in `OnAddFailed()` or `OnModifyFailed()`, which
        causes reentrant.

    @throw zaf::UnknownRuntimeError
        Thrown if failed to modify the icon.

    @throw ...
        Any exception will be thrown by `OnModifyFailed()` if derived class overrides it.

    @details
        If the icon has been added, this method will try to modify the icon in the system 
        notification area. If the icon is failed to be modified, `OnModifyFailed()` will be called.
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

    @throw zaf::InvalidOperationError
        Thrown if calling this method directly in `OnAddFailed()` or `OnModifyFailed()`, which
        causes reentrant.

    @throw zaf::UnknownRuntimeError
        Thrown if failed to modify the icon.

    @throw ...
        Any exception will be thrown by `OnModifyFailed()` if derived class overrides it.
        
    @details
        If the icon has been added, this method will try to modify the icon in the system 
        notification area. If the icon is failed to be modified, `OnModifyFailed()` will be called.
    */
    void SetTooltip(std::wstring tooltip);

    /**
    Adds the icon to the system notification area.

    @throw zaf::InvalidOperationError
        Thrown if calling this method directly in `OnAddFailed()` or `OnModifyFailed()`, which 
        causes reentrant.

    @throw zaf::Win32Error
        Thrown if failed to create the message window for the icon.

    @throw zaf::UnknownRuntimeError
        Thrown if failed to add the icon.

    @throw ...
        Any exception will be thrown by `OnAddFailed()` if derived class overrides it.

    @details
        This method will try to remove the existing icon before adding a new one. If the icon is 
        failed to be added, `OnAddFailed()` will be called. 
    */
    void Add();

    /**
    Removes the icon from the system notification area.

    @return
        `true` if the icon was successfully removed, `false` otherwise.
    */
    bool Remove() noexcept;

    /**
    Gets the event that is raised when the icon is activated, such as clicked.
    */
    rx::Observable<TrayIconActivateInfo> ActivateEvent() const;

    /**
    Gets the event that is raised when the context menu of the icon is requested,
    such as right-clicked.
    */
    rx::Observable<TrayIconContextMenuInfo> ContextMenuEvent() const;

protected:
    /**
    Called when the icon is failed to be added.

    @details
        The default implementation of this method does nothing. Derived classes can override this 
        method to handle the failure, such as retry after a period.
    */
    virtual void OnAddFailed();

    /**
    Called when the icon is failed to be modified.

    @details
        The default implementation of this method does nothing. Derived classes can override this 
        method to handle the failure, such as re-add the icon after a period.
    */
    virtual void OnModifyFailed();

    /**
    Called when the icon is activated, such as clicked.

    @param event_info
        The information about the activation event.

    @details
        The default implementation of this method raises the `ActivateEvent()`. Derived classes 
        should call the base implementation if they override this method.
    */
    virtual void OnActivate(const TrayIconActivateInfo& event_info);

    /**
    Called when the context menu of the icon is requested, such as right-clicked.

    @param event_info
        The information about the context menu event.

    @details
        The default implementation of this method raises the `ContextMenuEvent()`. Derived classes 
        should call the base implementation if they override this method.
    */
    virtual void OnContextMenu(const TrayIconContextMenuInfo& event_info);

private:
    /**
    Indicates the state of the icon in the system notification area.

    @details
        Since the icon is managed by the shell process, and the `Shell_NotifyIcon` doesn't return
        a meaningful error code nor provide a way to query the state of the icon, we have to assume
        the state of the icon is always intermediate even after a successful call to 
        `Shell_NotifyIcon`.
    */
    enum class IconState {
        NotAdded,
        Intermediate,
    };

private:
    void InitializeIfNeeded();
    void AddIcon();
    bool CallAdd() const noexcept;
    bool CallSetVersion() const noexcept;
    void ModifyIcon(NOTIFYICONDATA& icon_data);
    NOTIFYICONDATA MakeBasicIconData() const noexcept;
    AutoReset<bool> BeginOperatingIcon();
    void OnMessageReceived(const Message& message);
    void HandleSELECT(const Message& message);
    void HandleWMCONTEXTMENU(const Message& message);

private:
    GUID id_{};
    std::wstring tooltip_;
    UniqueHICON icon_;
    std::unique_ptr<MessageOnlyWindow> message_window_;

    IconState icon_state_{ IconState::NotAdded };
    bool is_operating_icon_{};

    rx::Subject<TrayIconActivateInfo> activate_event_;
    rx::Subject<TrayIconContextMenuInfo> context_menu_event_;
};

ZAF_OBJECT_BEGIN(TrayIcon)
ZAF_OBJECT_END

}