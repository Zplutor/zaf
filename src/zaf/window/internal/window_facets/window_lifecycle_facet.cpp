#include <zaf/window/internal/window_facets/window_lifecycle_facet.h>
#include <zaf/application.h>
#include <zaf/base/auto_reset.h>
#include <zaf/base/error/win32_error.h>
#include <zaf/control/control.h>
#include <zaf/graphic/dpi.h>
#include <zaf/internal/window/window_focused_control_manager.h>
#include <zaf/window/internal/window_facets/window_focus_facet.h>
#include <zaf/window/internal/window_facets/window_geometry_facet.h>
#include <zaf/window/internal/window_facets/window_mouse_facet.h>
#include <zaf/window/internal/window_facets/window_render_facet.h>
#include <zaf/window/invalid_handle_state_error.h>
#include <zaf/window/tooltip_window.h>
#include <zaf/window/window.h>
#include <zaf/window/window_class.h>
#include <zaf/window/window_holder.h>

namespace zaf::internal {
namespace {

struct CreateWindowParams {
    WindowLifecycleFacet* lifecycle_facet{};
    const internal::WindowNotCreatedStateData* not_created_state_data{};
};

} // namespace

void WindowLifecycleFacet::HandleWMNCCREATE(HWND handle, LPARAM lparam) noexcept {

    auto create_struct = reinterpret_cast<const CREATESTRUCTA*>(lparam);
    auto create_params =
        reinterpret_cast<const CreateWindowParams*>(create_struct->lpCreateParams);

    SetWindowLongPtr(
        handle, 
        GWLP_USERDATA, 
        reinterpret_cast<ULONG_PTR>(&create_params->lifecycle_facet->window_));

    create_params->lifecycle_facet->HandleStateData().handle = handle;
}


WindowLifecycleFacet::WindowLifecycleFacet(Window& window) noexcept : window_(window) {

}


WindowHandleState WindowLifecycleFacet::HandleState() const noexcept {
    return handle_state_;
}


HWND WindowLifecycleFacet::Handle() const noexcept {
    if (handle_state_ == WindowHandleState::Creating ||
        handle_state_ == WindowHandleState::Created ||
        handle_state_ == WindowHandleState::Destroying) {
        return HandleStateData().handle;
    }
    return nullptr;
}


std::shared_ptr<WindowHolder> WindowLifecycleFacet::Holder() const noexcept {
    return holder_.lock();
}


WindowNotCreatedStateData& WindowLifecycleFacet::NotCreatedStateData() noexcept {
    return std::get<WindowNotCreatedStateData>(state_data_);
}


const WindowNotCreatedStateData& WindowLifecycleFacet::NotCreatedStateData() const noexcept {
    return std::get<WindowNotCreatedStateData>(state_data_);
}


WindowHandleStateData& WindowLifecycleFacet::HandleStateData() noexcept {
    return std::get<WindowHandleStateData>(state_data_);
}


const WindowHandleStateData& WindowLifecycleFacet::HandleStateData() const noexcept {
    return std::get<WindowHandleStateData>(state_data_);
}


std::shared_ptr<WindowHolder> WindowLifecycleFacet::CreateHandle() {

    if (handle_state_ == WindowHandleState::NotCreated) {
        return CreateHandleInNotCreatedState();
    }
    else if (handle_state_ == WindowHandleState::Created) {
        return holder_.lock();
    }
    else {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }
}


std::shared_ptr<WindowHolder> WindowLifecycleFacet::CreateHandleInNotCreatedState() {

    // Check if the owner's handle state is valid.
    auto owner = window_.Owner();
    if (owner) {
        auto owner_handle_state = owner->HandleState();
        if (owner_handle_state == WindowHandleState::NotCreated ||
            owner_handle_state == WindowHandleState::Destroying ||
            owner_handle_state == WindowHandleState::Destroyed) {
            throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
        }
    }

    // The window must be kept alive in this method using the shared pointer, as in the following
    // DestroyedEvent callback, user codes may release the window pointer they kept, which causes 
    // `this` pointer to be dangling after the DestroyedEvent callback returns, and then the code
    // accessing members will crash.
    auto shared_window = window_.shared_from_this();

    auto holder = std::make_shared<WindowHolder>(shared_window);
    holder_ = holder;

    handle_state_ = WindowHandleState::Creating;

    auto not_created_state_data = std::move(NotCreatedStateData());
    state_data_.emplace<internal::WindowHandleStateData>();

    try {

        ProcessCreatingState(*this, not_created_state_data, *window_.Class(), owner);

        handle_state_ = WindowHandleState::Created;
        ProcessCreatedState();

        return holder;
    }
    catch (...) {

        // If the state is Destroyed, it means Destroy method has been called during the 
        // creation, so don't reset the state to NotCreated.
        if (handle_state_ != WindowHandleState::Destroyed) {

            auto auto_reset = MakeAutoReset(destroy_reason_, DestroyReason::CreationFailed);
            window_.Destroy();

            handle_state_ = WindowHandleState::NotCreated;
            state_data_ = std::move(not_created_state_data);
        }

        // The holder member should be reset last, as it is used in the Destroy() method to 
        // detach the window.
        holder_.reset();
        throw;
    }
}


// Do not access any member of instance, as it is in a middle state of creation.
void WindowLifecycleFacet::ProcessCreatingState(
    WindowLifecycleFacet& lifecycle_facet,
    const internal::WindowNotCreatedStateData& state_data,
    const WindowClass& window_class,
    const std::shared_ptr<Window>& owner) {

    DWORD style = state_data.basic_style.Value();
    DWORD extended_style = state_data.extended_style.Value();

    auto screen = WindowGeometryFacet::ResolveInitialScreen(state_data, owner);
    auto initial_rect = WindowGeometryFacet::ResolveInitialRect(
        *screen,
        owner,
        state_data);
    auto initial_rect_in_global = screen->TransformToGlobal(initial_rect);

    CreateWindowParams params;
    params.lifecycle_facet = &lifecycle_facet;
    params.not_created_state_data = &state_data;

    // During the execution of CreateWindowEx, the window handle will be set to handle_ member
    // when the WM_NCCREATE message is received.
    auto handle = CreateWindowEx(
        extended_style,
        reinterpret_cast<LPCWSTR>(window_class.GetAtom()),
        state_data.title.c_str(),
        style,
        static_cast<int>(initial_rect_in_global.position.x),
        static_cast<int>(initial_rect_in_global.position.y),
        static_cast<int>(initial_rect_in_global.size.width),
        static_cast<int>(initial_rect_in_global.size.height),
        owner ? owner->Handle() : nullptr,
        nullptr,
        nullptr,
        &params);

    if (!handle) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }
}


void WindowLifecycleFacet::HandleWMCREATE() {
    window_.OnHandleCreating(HandleCreatingInfo{ window_.shared_from_this() });
}


void WindowLifecycleFacet::ProcessCreatedState() {

    // Call SetWindowPos to re-calculate the non-client area if custom frame is used.
    if (window_.UseCustomFrame()) {
        SetWindowPos(
            window_.Handle(),
            nullptr,
            0, 
            0,
            0,
            0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }

    auto handle = Handle();
    auto dpi = static_cast<float>(GetDpiForWindow(handle));

    RECT client_rect{};
    GetClientRect(handle, &client_rect);
    window_.RootControl()->SetRect(ToDIPs(Rect::FromRECT(client_rect), dpi));

    window_.render_facet_->CreateRenderer();

    window_.OnHandleCreated(HandleCreatedInfo{ window_.shared_from_this() });
}


void WindowLifecycleFacet::Close() noexcept {

    if (handle_state_ == WindowHandleState::NotCreated) {
        this->Destroy();
    }
    else if (handle_state_ == WindowHandleState::Creating ||
             handle_state_ == WindowHandleState::Created) {
        SendMessage(Handle(), WM_CLOSE, 0, 0);
    }
}


void WindowLifecycleFacet::HandleWMCLOSE() {

    ClosingInfo event_info{ window_.shared_from_this() };
    window_.OnClosing(event_info);

    if (event_info.CanClose()) {
        auto auto_reset = MakeAutoReset(destroy_reason_, DestroyReason::Closed);
        Destroy();
    }
}


void WindowLifecycleFacet::Destroy() noexcept {

    if (handle_state_ == WindowHandleState::NotCreated) {

        handle_state_ = WindowHandleState::Destroyed;
        window_.OnDestroyed(DestroyedInfo{ 
            window_.shared_from_this(), 
            nullptr, 
            DestroyReason::Unspecified 
        });
    }
    else if (handle_state_ == WindowHandleState::Creating ||
             handle_state_ == WindowHandleState::Created) {

        // Handle related resources would be released in the WM_DESTROY message handler.
        // OnDestroy method would be called in the WM_DESTROY message handler as well.
        DestroyWindow(Handle());
    }
}


void WindowLifecycleFacet::HandleWMDESTROY() {

    //Avoid reentering.
    if (handle_state_ == WindowHandleState::Destroying) {
        return;
    }

    handle_state_ = WindowHandleState::Destroying;
    window_.OnDestroying(DestroyingInfo{ window_.shared_from_this(), destroy_reason_ });

    window_.mouse_facet_->CancelMouseCapture();
    window_.focus_facet_->HandleWindowDestroy();
    window_.RootControl()->ReleaseRendererResources();

    HWND old_handle = Handle();

    auto& state_data = HandleStateData();
    state_data.renderer = {};
    if (state_data.tooltip_window) {
        state_data.tooltip_window->Destroy();
        state_data.tooltip_window.reset();
    }

    state_data_.emplace<std::monostate>();
    handle_state_ = WindowHandleState::Destroyed;
    window_.OnDestroyed(DestroyedInfo{ window_.shared_from_this(), old_handle, destroy_reason_ });
}


void WindowLifecycleFacet::HandleWMNCDESTROY() {

    auto holder = holder_.lock();
    if (holder) {

        Application::Instance().UnregisterShownWindow(holder);

        holder->Detach();
        holder_.reset();
    }
}

}