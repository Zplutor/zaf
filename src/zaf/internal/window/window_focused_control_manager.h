#pragma once

#include <zaf/base/event/event.h>
#include <zaf/base/non_copyable.h>
#include <zaf/control/control.h>
#include <zaf/window/window.h>

namespace zaf::internal {

class WindowFocusedControlManager : NonCopyable {
public:
    explicit WindowFocusedControlManager(Window& window);

    const std::shared_ptr<Control>& FocusedControl() const {
        return focused_control_;
    }

    bool TryToPreprocessTabKeyMessage(const KeyMessage& message);
    void HandleWindowSetFocus();
    void HandleWindowKillFocus();
    void HandleWindowDestroy();

    void ChangeFocusedControl(const std::shared_ptr<Control>& new_focused_control);

    Observable<std::shared_ptr<Control>> FocusedControlChangedEvent() const {
        return focused_control_changed_event_.GetObservable();
    }

private:
    void SwitchFocusedControlByTabKey(bool backward);

    bool CheckIfCanChangeFocusedControl(const std::shared_ptr<Control>& new_focused_control) const;

    static void ChangeControlFocusState(
        const std::shared_ptr<Control>& target_control,
        const std::shared_ptr<Control>& changing_control,
        bool is_focused);


private:
    Window& window_;

    std::size_t change_counter_{};
    std::shared_ptr<Control> focused_control_;
    std::weak_ptr<Control> last_focused_control_;

    Event<std::shared_ptr<Control>> focused_control_changed_event_;
};

}