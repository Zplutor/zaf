#include <zaf/window/tray_icon.h>
#include <zaf/application.h>
#include <zaf/base/log.h>
#include <zaf/base/auto_reset.h>
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

    if (!has_added_) {
        return;
    }

    auto icon_data = MakeBasicIconData();
    icon_data.uFlags |= NIF_ICON;
    icon_data.hIcon = icon_;
    BOOL is_succeeded = Shell_NotifyIcon(NIM_MODIFY, &icon_data);
    if (!is_succeeded) {
        OnModifyFailed();
    }
}


const std::wstring& TrayIcon::Tooltip() const noexcept {
    return tooltip_;
}


void TrayIcon::SetTooltip(std::wstring tooltip) {

    tooltip_ = std::move(tooltip);

    if (!has_added_) {
        return;
    }

    auto icon_data = MakeBasicIconData();
    icon_data.uFlags |= NIF_TIP;
    wcscpy_s(icon_data.szTip, tooltip_.c_str());
    BOOL is_succeeded = Shell_NotifyIcon(NIM_MODIFY, &icon_data);
    if (!is_succeeded) {
        OnModifyFailed();
    }
}


void TrayIcon::Add() {

    if (is_adding_) {
        throw InvalidOperationError(ZAF_SOURCE_LOCATION());
    }

    auto auto_reset = MakeAutoReset(is_adding_, true);

    if (!message_window_) {
        // This may throw `zaf::Win32Error` if failed to create the message window.
        message_window_.emplace();
        Disposables() += message_window_->MessageReceivedEvent().Subscribe(
            std::bind_front(&TrayIcon::OnMessageReceived, this));
    }

    auto icon_data = MakeBasicIconData();
    icon_data.uFlags |= NIF_MESSAGE | NIF_TIP | NIF_ICON;
    icon_data.hWnd = message_window_->Handle();
    icon_data.uCallbackMessage = TrayIconMessageID;
    icon_data.hIcon = icon_;
    wcscpy_s(icon_data.szTip, tooltip_.c_str());

    BOOL is_succeeded = Shell_NotifyIcon(NIM_ADD, &icon_data);
    if (!is_succeeded) {
        OnAddFailed();
    }

    has_added_ = true;
}


bool TrayIcon::Remove() noexcept {

    auto icon_data = MakeBasicIconData();
    BOOL is_succeeded = Shell_NotifyIcon(NIM_DELETE, &icon_data);
    has_added_ = false;
    return !!is_succeeded;
}


NOTIFYICONDATA TrayIcon::MakeBasicIconData() const noexcept {
    NOTIFYICONDATA icon_data{};
    icon_data.cbSize = sizeof(NOTIFYICONDATA);
    icon_data.uVersion = NOTIFYICON_VERSION_4;
    icon_data.uFlags = NIF_GUID;
    icon_data.guidItem = id_;
    return icon_data;
}


void TrayIcon::OnMessageReceived(const Message& message) {

    if (message.ID() != TrayIconMessageID) {
        return;
    }

    ZAF_LOG() << "LOWORD(LParam()) " << LOWORD(message.LParam());

    switch (LOWORD(message.LParam())) {
    // single click (keyboard or mouse)
    case NIN_SELECT:        
        ZAF_LOG() << L"Selected";
        break;

     // keyboard activation
    case NIN_KEYSELECT:     
        ZAF_LOG() << "Key Selected";
        break;

    // right-click
    case WM_CONTEXTMENU: {
        ZAF_LOG() << "Context menu requested";
        break;
    }

    // double click
    case WM_LBUTTONDBLCLK:  
        ZAF_LOG() << "Double clicked";
        break;

    case NIN_BALLOONUSERCLICK:
        break;

    // hover (Vista+)
    case NIN_POPUPOPEN:     
        break;

    case NIN_POPUPCLOSE:
        break;
    }
}


void TrayIcon::OnAddFailed() {
    throw UnknownRuntimeError(ZAF_SOURCE_LOCATION());
}


void TrayIcon::OnModifyFailed() {
    throw UnknownRuntimeError(ZAF_SOURCE_LOCATION());
}

}