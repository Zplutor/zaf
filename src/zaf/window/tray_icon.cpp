#include <zaf/window/tray_icon.h>
#include <windowsx.h>
#include <zaf/application.h>
#include <zaf/base/log.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/base/error/unknown_runtime_error.h>

namespace zaf {
namespace {

constexpr UINT TrayIconMessageID = WM_USER;

}

ZAF_OBJECT_IMPL(TrayIcon)

TrayIcon::TrayIcon(const GUID& id) noexcept : id_(id) {

}


TrayIcon::~TrayIcon() {
    Remove();
}


const GUID& TrayIcon::ID() const noexcept {
    return id_;
}


const UniqueHICON& TrayIcon::Icon() const noexcept {
    return icon_;
}


void TrayIcon::SetIcon(UniqueHICON icon) {

    icon_ = std::move(icon);
    if (icon_state_ == IconState::NotAdded) {
        return;
    }

    auto icon_data = MakeBasicIconData();
    icon_data.uFlags |= NIF_ICON;
    icon_data.hIcon = icon_;
    ModifyIcon(icon_data);
}


const std::wstring& TrayIcon::Tooltip() const noexcept {
    return tooltip_;
}


void TrayIcon::SetTooltip(std::wstring tooltip) {

    tooltip_ = std::move(tooltip);
    if (icon_state_ == IconState::NotAdded) {
        return;
    }

    auto icon_data = MakeBasicIconData();
    icon_data.uFlags |= NIF_TIP;
    wcscpy_s(icon_data.szTip, tooltip_.c_str());
    ModifyIcon(icon_data);
}


void TrayIcon::Add() {

    auto auto_reset = BeginOperatingIcon();

    // This may throw `zaf::Win32Error` if failed to create the message window.
    InitializeIfNeeded();

    if (icon_state_ == IconState::NotAdded) {
        AddIcon();
    }
    else {
        Remove();
        AddIcon();
    }
}


void TrayIcon::InitializeIfNeeded() {

    if (message_window_) {
        return;
    }

    auto window = std::make_unique<MessageOnlyWindow>();
    Disposables() += window->MessageReceivedEvent().Subscribe(
        std::bind_front(&TrayIcon::OnMessageReceived, this));

    message_window_ = std::move(window);
}


void TrayIcon::AddIcon() {

    if (!CallAdd()) {
        OnAddFailed();
        throw UnknownRuntimeError(ZAF_SOURCE_LOCATION());
    }

    icon_state_ = IconState::Intermediate;

    if (!CallSetVersion()) {
        // Set version fail, we should remove the icon.
        Remove();
        OnAddFailed();
        throw UnknownRuntimeError(ZAF_SOURCE_LOCATION());
    }
}


bool TrayIcon::CallAdd() const noexcept {

    auto icon_data = MakeBasicIconData();
    icon_data.uFlags |= NIF_MESSAGE | NIF_TIP | NIF_ICON;
    icon_data.hWnd = message_window_->Handle();
    icon_data.uCallbackMessage = TrayIconMessageID;
    icon_data.hIcon = icon_;
    wcscpy_s(icon_data.szTip, tooltip_.c_str());

    BOOL is_succeeded = Shell_NotifyIcon(NIM_ADD, &icon_data);
    return !!is_succeeded;
}


bool TrayIcon::CallSetVersion() const noexcept {

    auto icon_data = MakeBasicIconData();
    BOOL is_succeeded = Shell_NotifyIcon(NIM_SETVERSION, &icon_data);
    return !!is_succeeded;
}


void TrayIcon::OnAddFailed() {

}


void TrayIcon::ModifyIcon(NOTIFYICONDATA& icon_data) {

    auto auto_reset = BeginOperatingIcon();

    BOOL is_succeeded = Shell_NotifyIcon(NIM_MODIFY, &icon_data);
    if (!is_succeeded) {
        OnModifyFailed();
        throw UnknownRuntimeError(ZAF_SOURCE_LOCATION());
    }
}


void TrayIcon::OnModifyFailed() {

}


bool TrayIcon::Remove() noexcept {

    // If the icon is not added, there is no need to call Shell_NotifyIcon.
    if (icon_state_ == IconState::NotAdded) {
        return true;
    }

    auto icon_data = MakeBasicIconData();
    BOOL is_succeeded = Shell_NotifyIcon(NIM_DELETE, &icon_data);
    if (is_succeeded) {
        icon_state_ = IconState::NotAdded;
        return true;
    }
    else {
        // If failed to remove the icon, it may be because either the icon was not added or the IPC
        // call to the shell process failed.
        icon_state_ = IconState::Intermediate;
        return false;
    }
}


NOTIFYICONDATA TrayIcon::MakeBasicIconData() const noexcept {
    NOTIFYICONDATA icon_data{};
    icon_data.cbSize = sizeof(NOTIFYICONDATA);
    icon_data.uVersion = NOTIFYICON_VERSION_4;
    icon_data.uFlags = NIF_GUID;
    icon_data.guidItem = id_;
    return icon_data;
}


AutoReset<bool> TrayIcon::BeginOperatingIcon() {
    if (is_operating_icon_) {
        throw InvalidOperationError(ZAF_SOURCE_LOCATION());
    }
    return MakeAutoReset(is_operating_icon_, true);
}


void TrayIcon::OnMessageReceived(const Message& message) {

    if (message.ID() != TrayIconMessageID) {
        return;
    }

    switch (LOWORD(message.LParam())) {
    case NIN_SELECT:        
    case NIN_KEYSELECT: 
        HandleSELECT(message);
        break;

    case WM_CONTEXTMENU: 
        HandleWMCONTEXTMENU(message);
        break;
    }
}


void TrayIcon::HandleSELECT(const Message& message) {
    TrayIconActivateInfo event_info{ shared_from_this() };
    OnActivate(event_info);
}


void TrayIcon::HandleWMCONTEXTMENU(const Message& message) {

    auto x = GET_X_LPARAM(message.WParam());
    auto y = GET_Y_LPARAM(message.WParam());

    TrayIconContextMenuInfo event_info{ 
        shared_from_this(), 
        Point{ static_cast<float>(x), static_cast<float>(y) }
    };
    OnContextMenu(event_info);
}


rx::Observable<TrayIconActivateInfo> TrayIcon::ActivateEvent() const {
    return activate_event_.AsObservable();
}


void TrayIcon::OnActivate(const TrayIconActivateInfo& event_info) {
    activate_event_.AsObserver().OnNext(event_info);
}


rx::Observable<TrayIconContextMenuInfo> TrayIcon::ContextMenuEvent() const {
    return context_menu_event_.AsObservable();
}


void TrayIcon::OnContextMenu(const TrayIconContextMenuInfo& event_info) {
    context_menu_event_.AsObserver().OnNext(event_info);
}

}