#include <zaf/window/internal/window_facets/window_visibility_facet.h>
#include <zaf/application.h>
#include <zaf/window/internal/window_facets/window_lifecycle_facet.h>
#include <zaf/window/invalid_handle_state_error.h>
#include <zaf/window/window.h>

namespace zaf::internal {

WindowVisibilityFacet::WindowVisibilityFacet(Window& window) noexcept : window_(window) {

}


void WindowVisibilityFacet::Show(ShowOptions options) {

    bool no_activate =
        HasFlag(options, ShowOptions::NoActivate) ||
        HasFlag(window_.ActivateOptions(), zaf::ActivateOptions::NoAutoActivate);

    InnerShowWindow(no_activate ? SW_SHOWNA : SW_SHOW);
}


void WindowVisibilityFacet::Maximize() {
    InnerShowWindow(SW_SHOWMAXIMIZED);
}


void WindowVisibilityFacet::Minimize() {
    InnerShowWindow(SW_SHOWMINIMIZED);
}


void WindowVisibilityFacet::Restore() {
    InnerShowWindow(SW_RESTORE);
}


void WindowVisibilityFacet::InnerShowWindow(int show_command) {

    std::shared_ptr<WindowHolder> holder;

    auto handle_state = window_.HandleState();
    if (handle_state == WindowHandleState::NotCreated) {
        holder = window_.CreateHandle();
    }
    else if (handle_state == WindowHandleState::Creating ||
             handle_state == WindowHandleState::Created) {
        holder = window_.lifecycle_facet_->Holder();
    }
    else {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }

    Application::Instance().RegisterShownWindow(holder);
    ShowWindow(window_.Handle(), show_command);
}


void WindowVisibilityFacet::Hide() noexcept {

    auto handle_state = window_.HandleState();
    if (handle_state == WindowHandleState::Created ||
        handle_state == WindowHandleState::Creating) {
        ShowWindow(window_.Handle(), SW_HIDE);
    }
}


bool WindowVisibilityFacet::IsVisible() const noexcept {

    auto handle_state = window_.HandleState();
    if (handle_state == WindowHandleState::Creating ||
        handle_state == WindowHandleState::Created ||
        handle_state == WindowHandleState::Destroying) {
        return !!IsWindowVisible(window_.Handle());
    }
    return false;
}


bool WindowVisibilityFacet::IsWindowMaximized() const noexcept {
    auto handle = window_.Handle();
    if (handle) {
        return IsMaximized(handle);
    }
    return false;
}


bool WindowVisibilityFacet::IsWindowMinimized() const noexcept {
    auto handle = window_.Handle();
    if (handle) {
        return IsMinimized(handle);
    }
    return false;
}

}