#pragma once

#include <variant>
#include <zaf/base/non_copyable.h>
#include <zaf/window/event/destroy_infos.h>
#include <zaf/window/internal/window_handle_state_data.h>
#include <zaf/window/internal/window_not_created_state_data.h>
#include <zaf/window/window_handle_state.h>

namespace zaf {
class Window;
class WindowClass;
class WindowHolder;
}

namespace zaf::internal {

class WindowLifecycleFacet : public NonCopyableNonMovable {
public:
    static void HandleWMNCCREATE(HWND handle, LPARAM lparam) noexcept;

public:
    explicit WindowLifecycleFacet(Window& window) noexcept;

    WindowHandleState HandleState() const noexcept;
    HWND Handle() const noexcept;
    std::shared_ptr<WindowHolder> Holder() const noexcept;

    WindowNotCreatedStateData& NotCreatedStateData() noexcept;
    const WindowNotCreatedStateData& NotCreatedStateData() const noexcept;

    WindowHandleStateData& HandleStateData() noexcept;
    const WindowHandleStateData& HandleStateData() const noexcept;

    std::shared_ptr<WindowHolder> CreateHandle();
    void HandleWMCREATE();
    void Close() noexcept;
    void HandleWMCLOSE();
    void Destroy() noexcept;
    void HandleWMDESTROY();
    void HandleWMNCDESTROY();

private:
    std::shared_ptr<WindowHolder> CreateHandleInNotCreatedState();

    static void ProcessCreatingState(
        WindowLifecycleFacet& lifecycle_facet,
        const internal::WindowNotCreatedStateData& state_data,
        const WindowClass& window_class,
        const std::shared_ptr<Window>& owner);

    void ProcessCreatedState();

private:
    Window& window_;

    WindowHandleState handle_state_{ WindowHandleState::NotCreated };

    /*
    State data type for different window handle states:
    - NotCreated: WindowNotCreatedStateData
    - Creating, Created, Destroying: WindowHandleStateData
    - Destroyed: std::monostate
    */
    std::variant<
        internal::WindowNotCreatedStateData,
        internal::WindowHandleStateData,
        std::monostate
    > state_data_;

    std::weak_ptr<WindowHolder> holder_;
    DestroyReason destroy_reason_{ DestroyReason::Unspecified };
};

}