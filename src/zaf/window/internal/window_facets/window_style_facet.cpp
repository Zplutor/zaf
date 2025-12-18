#include <zaf/window/internal/window_facets/window_style_facet.h>
#include <zaf/base/error/precondition_error.h>
#include <zaf/base/error/win32_error.h>
#include <zaf/window/internal/window_facets/window_lifecycle_facet.h>
#include <zaf/window/internal/window_style_shim.h>
#include <zaf/window/invalid_handle_state_error.h>
#include <zaf/window/window.h>

namespace zaf::internal {

WindowStyleFacet::WindowStyleFacet(Window& window, std::shared_ptr<WindowClass> window_class) :
    window_(window),
    class_(std::move(window_class)) {

    ZAF_EXPECT(class_);
}


std::wstring WindowStyleFacet::Title() const {

    auto handle_state = window_.HandleState();
    if (handle_state == WindowHandleState::NotCreated) {
        return window_.lifecycle_facet_->NotCreatedStateData().title;
    }
    else if (handle_state == WindowHandleState::Creating ||
             handle_state == WindowHandleState::Created ||
             handle_state == WindowHandleState::Destroying) {

        const auto& handle_state = window_.lifecycle_facet_->HandleStateData();

        SetLastError(0);
        int title_length = GetWindowTextLength(handle_state.handle);
        if (title_length == 0) {
            auto last_error = GetLastError();
            if (last_error) {
                ZAF_THROW_WIN32_ERROR(last_error);
            }
            return {};
        }

        std::wstring title;
        title.resize(title_length + 1);

        int actual_length = GetWindowText(
            handle_state.handle,
            title.data(),
            static_cast<int>(title.size()));

        if (actual_length == 0) {
            ZAF_THROW_WIN32_ERROR(GetLastError());
        }
        title.resize(actual_length);
        return title;
    }
    else {
        return {};
    }
}


void WindowStyleFacet::SetTitle(const std::wstring& title) {

    auto handle_state = window_.HandleState();
    if (handle_state == WindowHandleState::NotCreated) {
        window_.lifecycle_facet_->NotCreatedStateData().title = title;
    }
    else if (handle_state == WindowHandleState::Creating ||
             handle_state == WindowHandleState::Created) {

        BOOL is_succeeded = SetWindowText(window_.Handle(), title.c_str());
        if (!is_succeeded) {
            ZAF_THROW_WIN32_ERROR(GetLastError());
        }
    }
    else {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }
}


template<typename PROPERTY>
bool WindowStyleFacet::HasStyleProperty(PROPERTY property) const noexcept {

    auto handle_state = window_.HandleState();
    if (handle_state == WindowHandleState::NotCreated) {

        return internal::WindowStyleShim<PROPERTY>::Has(
            window_.lifecycle_facet_->NotCreatedStateData(),
            property);
    }

    if (handle_state == WindowHandleState::Creating ||
        handle_state == WindowHandleState::Created ||
        handle_state == WindowHandleState::Destroying) {

        return internal::WindowStyleShim<PROPERTY>::Has(
            window_.lifecycle_facet_->HandleStateData(),
            property);
    }

    return false;
}


template<typename PROPERTY>
void WindowStyleFacet::SetStyleProperty(
    PROPERTY property, 
    bool enable, 
    bool can_set_if_has_handle) {

    auto handle_state = window_.HandleState();
    if (handle_state == WindowHandleState::NotCreated) {

        internal::WindowStyleShim<PROPERTY>::Set(
            window_.lifecycle_facet_->NotCreatedStateData(), 
            property, 
            enable);
    }
    else if (handle_state == WindowHandleState::Creating ||
             handle_state == WindowHandleState::Created) {

        if (!can_set_if_has_handle) {
            throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
        }

        internal::WindowStyleShim<PROPERTY>::Set(
            window_.lifecycle_facet_->HandleStateData(), 
            property,
            enable);
    }
    else {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }
}


bool WindowStyleFacet::IsPopup() const noexcept {
    return HasStyleProperty(internal::WindowBasicStyleProperty::IsPopup);
}


void WindowStyleFacet::SetIsPopup(bool is_popup) {
    SetStyleProperty(internal::WindowBasicStyleProperty::IsPopup, is_popup, false);
}


bool WindowStyleFacet::HasBorder() const noexcept {
    return HasStyleProperty(internal::WindowBasicStyleProperty::HasBorder);
}


void WindowStyleFacet::SetHasBorder(bool has_border) {
    SetStyleProperty(internal::WindowBasicStyleProperty::HasBorder, has_border, false);
}


bool WindowStyleFacet::HasTitleBar() const noexcept {
    return HasStyleProperty(internal::WindowBasicStyleProperty::HasTitleBar);
}


void WindowStyleFacet::SetHasTitleBar(bool has_title_bar) {
    SetStyleProperty(internal::WindowBasicStyleProperty::HasTitleBar, has_title_bar, false);
}


bool WindowStyleFacet::HasSystemMenu() const noexcept {
    return HasStyleProperty(internal::WindowBasicStyleProperty::HasSystemMenu);
}


void WindowStyleFacet::SetHasSystemMenu(bool has_system_menu) {
    SetStyleProperty(internal::WindowBasicStyleProperty::HasSystemMenu, has_system_menu, false);
}


bool WindowStyleFacet::IsSizable() const noexcept {
    return HasStyleProperty(internal::WindowBasicStyleProperty::IsSizable);
}


void WindowStyleFacet::SetIsSizable(bool is_sizable) {
    SetStyleProperty(internal::WindowBasicStyleProperty::IsSizable, is_sizable, true);
}


bool WindowStyleFacet::CanMaximize() const noexcept {
    return HasStyleProperty(internal::WindowBasicStyleProperty::CanMaximize);
}


void WindowStyleFacet::SetCanMaximize(bool has_maximize_button) {
    SetStyleProperty(internal::WindowBasicStyleProperty::CanMaximize, has_maximize_button, true);
}


bool WindowStyleFacet::CanMinimize() const noexcept {
    return HasStyleProperty(internal::WindowBasicStyleProperty::CanMinimize);
}


void WindowStyleFacet::SetCanMinimize(bool can_minimize) {
    SetStyleProperty(internal::WindowBasicStyleProperty::CanMinimize, can_minimize, true);
}


bool WindowStyleFacet::IsToolWindow() const noexcept {
    return HasStyleProperty(internal::WindowExtendedStyleProperty::IsToolWindow);
}


void WindowStyleFacet::SetIsToolWindow(bool is_tool_window) {
    SetStyleProperty(internal::WindowExtendedStyleProperty::IsToolWindow, is_tool_window, true);
}


bool WindowStyleFacet::IsTopmost() const noexcept {
    return HasStyleProperty(internal::WindowExtendedStyleProperty::IsTopMost);
}


void WindowStyleFacet::SetIsTopmost(bool is_topmost) {

    auto handle_state = window_.HandleState();
    if (handle_state == WindowHandleState::NotCreated) {

        window_.lifecycle_facet_->NotCreatedStateData().extended_style.Set(
            internal::WindowExtendedStyleProperty::IsTopMost,
            is_topmost);
    }
    else if (handle_state == WindowHandleState::Creating ||
             handle_state == WindowHandleState::Created) {

        BOOL is_succeeded = SetWindowPos(
            window_.Handle(),
            is_topmost ? HWND_TOPMOST : HWND_NOTOPMOST,
            0,
            0,
            0,
            0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

        if (!is_succeeded) {
            ZAF_THROW_WIN32_ERROR(GetLastError());
        }
    }
    else {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }
}


ActivateOptions WindowStyleFacet::ActivateOptions() const noexcept {
    return activate_options_;
}


void WindowStyleFacet::SetActivateOptions(zaf::ActivateOptions options) {

    if (window_.HandleState() == WindowHandleState::Destroyed) {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }

    SetStyleProperty(
        internal::WindowExtendedStyleProperty::NoActivate,
        HasFlag(options, ActivateOptions::NoAutoActivate),
        true);

    activate_options_ = options;
}


bool WindowStyleFacet::UseCustomFrame() const noexcept {
    return use_custom_frame_;
}


void WindowStyleFacet::SetUseCustomFrame(bool use_custom_frame) noexcept {

    if (use_custom_frame_ == use_custom_frame) {
        return;
    }

    use_custom_frame_ = use_custom_frame;

    auto handle_state = window_.HandleState();
    if (handle_state == WindowHandleState::Creating || 
        handle_state == WindowHandleState::Created) {

        // Call SetWindowPos to re-calculate the non-client area.
        SetWindowPos(
            window_.Handle(),
            nullptr,
            0,
            0,
            0,
            0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
}


std::optional<LRESULT> WindowStyleFacet::HandleWMNCCALCSIZE(const Message& message) {

    // WM_NCCALCSIZE must be passed to default window procedure if wparam is FALSE, no matter if
    // the window uses custom frame, otherwise the window could have some odd behaviors.
    if (message.WParam() == FALSE) {
        return std::nullopt;
    }

    //Return 0 to remove the default window frame.
    return 0;
}

}