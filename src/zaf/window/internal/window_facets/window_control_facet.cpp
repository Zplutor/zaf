#include <zaf/window/internal/window_facets/window_control_facet.h>
#include <zaf/base/error/precondition_error.h>
#include <zaf/control/control.h>
#include <zaf/window/window.h>

namespace zaf::internal {

WindowControlFacet::WindowControlFacet(Window& window) noexcept : 
    window_(window),
    root_control_(Create<Control>()) {

}


void WindowControlFacet::OnWindowInitialize() {
    InstallRootControl(root_control_);
}


const std::shared_ptr<Control>& WindowControlFacet::RootControl() const noexcept {
    return root_control_;
}


void WindowControlFacet::SetRootControl(const std::shared_ptr<Control>& new_control) {

    ZAF_EXPECT(new_control);

    //The same root control is allowed to be set multiple times.
    if (root_control_ == new_control) {
        return;
    }

    if (new_control->Parent()) {
        throw InvalidOperationError(ZAF_SOURCE_LOCATION());
    }

    if (new_control->Window()) {
        throw InvalidOperationError(ZAF_SOURCE_LOCATION());
    }

    //Cancel the focused control before changing the root control.
    auto focused_control = window_.FocusedControl();
    if (focused_control) {
        focused_control->SetIsFocused(false);
    }

    auto old_control = UninstallRootControl();
    InstallRootControl(new_control);

    window_.OnRootControlChanged(RootControlChangedInfo{ 
        window_.shared_from_this(), 
        old_control 
    });
}


std::shared_ptr<Control> WindowControlFacet::UninstallRootControl() {
    
    if (!root_control_) {
        return nullptr;
    }

    auto control = std::move(root_control_);

    control->ReleaseRendererResources();
    control->SetWindow(nullptr);

    auto current_window = window_.shared_from_this();
    try {
        control->RaiseWindowChangedEvent(current_window);
    }
    catch (...) {
        // Rollback the window if an exception is thrown within the window changed event.
        control->SetWindow(current_window);
        throw;
    }
    return control;
}


void WindowControlFacet::InstallRootControl(std::shared_ptr<Control> control) {

    root_control_ = std::move(control);

    auto old_window = root_control_->Window();
    root_control_->SetWindow(window_.shared_from_this());
    root_control_->RaiseWindowChangedEvent(old_window);

    root_control_->SetRect(window_.ContentRect());
}

}